/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *     Shuah Khan <shuah.khan@hp.com>
 *     Richard White <richard.white@hp.com>
 *     Bryan Sutula <Bryan.Sutula@hp.com>
 *
 *
 * This file implements SSL connection management functionality
 * such as open, close, send, and receive.  It uses OpenSSL to do
 * most of it's work.
 *
 * The following functions are supported:
 *
 * oh_ssl_ctx_init()            - Creates a new SSL_CTX object
 * oh_ssl_ctx_free()            - Free an SSL_CTX object
 * oh_ssl_connect()             - Create and open a new ssl conection
 * oh_ssl_disconnect()          - Close and free an SSL connection
 * oh_ssl_read()                - Read from an SSL connection
 * oh_ssl_write()               - Write to an SSL connection
 */


/* OpenSSL and other header files */
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <oa_soap_ssl.h>


/**
 * oh_ssl_ctx_init
 *
 * Create a new SSL_CTX object as a framework for TLS/SSL enabled functions.
 * In particular:
 * - Creates a new CTX object with default option values
 * - Sets common compatibility options
 * - Sets the default locations for trusted CA certificates.
 *   SSL_CTX_set_default_verify_paths() is used to add system-wide default
 *   certificate paths to the verify CApath without having to specify a
 *   default location.  The intent is that the distribution's configured
 *   location will be used.
 *
 * Return value: pointer to SSL_CTX or NULL for failure
 **/
SSL_CTX         *oh_ssl_ctx_init()
{
        SSL_CTX         *ctx;

        ctx = SSL_CTX_new(SSLv23_client_method());
        if (ctx == NULL) {
                err("SSL_CTX_new() failed");
                return(NULL);
        }

        SSL_CTX_set_options(ctx, SSL_OP_TLS_ROLLBACK_BUG | SSL_OP_ALL);

        if (! SSL_CTX_set_default_verify_paths(ctx)) {
                err("SSL_CTX_set_default_verify_paths() failed");
                return(NULL);
        }

        return(ctx);
}


/**
 * oh_ssl_ctx_free
 * @ctx:        pointer to SSL_CTX as returned by oh_ssl_ctx_init()
 *
 * Free an SSL_CTX object
 *
 * Return value: 0 for success, -1 for failure
 **/
int             oh_ssl_ctx_free(SSL_CTX *ctx)
{
        if (ctx == NULL) {
                err("unexpected NULL ctx pointer");
                return(-1);
        }

        SSL_CTX_free(ctx);

        return(0);
}


/**
 * oh_ssl_connect
 * @hostname:   Name of target host.  Format:
 *                  "hostname:port" or "IPaddress:port"
 * @ctx:        pointer to SSL_CTX as returned by oh_ssl_ctx_init()
 * @timeout:    maximum number of seconds to wait for a connection to
 *              hostname, or zero to wait forever
 *
 * Create and open a new ssl conection to the specified host.
 *
 * Return value: pointer to BIO, or NULL for failure
 **/
BIO             *oh_ssl_connect(char *hostname, SSL_CTX *ctx, long timeout)
{
        BIO             *bio;
        SSL             *ssl;
        fd_set          readfds;
        fd_set          writefds;
        struct timeval  tv;
        int             fd;
        int             err;

        if (hostname == NULL) {
                err("NULL hostname in oh_ssl_connect()");
                return(NULL);
        }
        if (ctx == NULL) {
                err("NULL ctx in oh_ssl_connect()");
                return(NULL);
        }
        if (timeout < 0) {
                err("inappropriate timeout in oh_ssl_connect()");
                return(NULL);
        }

        /* Start with a new SSL BIO */
        bio = BIO_new_ssl_connect(ctx);
        if (bio == NULL) {
                err("BIO_new_ssl_connect() failed");
                return(NULL);
        }

        /* Set up connection parameters for this BIO */
        BIO_set_conn_hostname(bio, hostname);
        BIO_set_nbio(bio, 1);           /* Set underlying socket to
                                         * non-blocking mode
                                         */

        /* Set up SSL session parameters */
        BIO_get_ssl(bio, &ssl);
        if (ssl == NULL) {
                err("BIO_get_ssl() failed");
                BIO_free_all(bio);
                return(NULL);
        }
        SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

        /* Ready to open the connection.  Note that this call will probably
         * take a while, so we need to retry it, watching for a timeout.
         */
        while (1) {
                if (BIO_do_connect(bio) == 1) {
                        break;          /* Connection established */
                }
                if (! BIO_should_retry(bio)) { /* Hard error? */
                        err("BIO_do_connect() failed");
                        err("SSL error: %s",
                            ERR_reason_error_string(ERR_get_error()));
                        BIO_free_all(bio);
                        return(NULL);
                }

                /* Wait until there's a change in the socket's status or until
                 * the timeout period.
                 *
                 * Get underlying file descriptor, needed for the select call.
                 */
                fd = BIO_get_fd(bio, NULL);
                if (fd == -1) {
                        err("BIO isn't initialized in oh_ssl_connect()");
                        BIO_free_all(bio);
                        return(NULL);
                }

                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                if (BIO_should_read(bio)) {
                        FD_SET(fd, &readfds);
                }
                else if (BIO_should_write(bio)) {
                        FD_SET(fd, &writefds);
                }
                else {                  /* This is BIO_should_io_special().
                                         * Not sure what "special" needs to
                                         * wait for, but both read and write
                                         * seems to work without unnecessary
                                         * retries.
                                         */
                        FD_SET(fd, &readfds);
                        FD_SET(fd, &writefds);
                }
                if (timeout) {
                        tv.tv_sec = timeout;
                        tv.tv_usec = 0;
                        err = select(fd + 1, &readfds, &writefds, NULL, &tv);
                }
                else {                  /* No timeout */
                        err = select(fd + 1, &readfds, &writefds, NULL, NULL);
                }

                /* Evaluate select() return code */
                if (err < 0) {
                        err("error during select()");
                        BIO_free_all(bio);
                        return(NULL);
                }
                if (err == 0) {
                        err("connection timeout to %s", hostname);
                        BIO_free_all(bio);
                        return(NULL);   /* Timeout */
                }
        }

        /* TODO: Do I need to set the client or server mode here?  I don't
         * think so.
         */

        return(bio);
}


/**
 * oh_ssl_disconnect
 * @bio:        pointer to a BIO as returned by oh_ssl_connect()
 * @shutdown:   Selects a uni-directional or bi-directional SSL shutdown.
 *              See the SSL_shutdown() man page.
 *
 * Close the SSL connection and free the memory associated with it.
 *
 * Return value: 0 for success, -1 for failure
 **/
int             oh_ssl_disconnect(BIO *bio, enum OH_SSL_SHUTDOWN_TYPE shutdown)
{
        SSL             *ssl;
        int             ret;

        if (bio == NULL) {
                err("NULL bio in oh_ssl_disconnect()");
                return(-1);
        }

        /* Shut down the SSL connection.  This may involve a handshake with
         * the server.
         */
        BIO_get_ssl(bio, &ssl);
        if (ssl == NULL) {
                err("BIO_get_ssl() failed");
                return(-1);
        }
        ret = SSL_shutdown(ssl);
        if (ret == -1) {
                err("SSL_shutdown() failed");
                /* Continuing on to free BIO memory */
        }
        else if ((ret == 0) && (shutdown == OH_SSL_BI)) {
                /* Still need stage 2 shutdown (see SSL_shutdown() man page) */
                ret = SSL_shutdown(ssl);
                if (ret == -1) {
                        err("SSL_shutdown() failed");
                        /* Continuing on to free BIO memory */
                }
                else if (ret == 0) {
                        err("stage 2 of SSL_shutdown() failed");
                        /* Continuing on to free BIO memory */
                }
        }

        /* Free the connection */
        BIO_free_all(bio);

        return(0);
}


/**
 * oh_ssl_read
 * @bio:        pointer to a BIO as returned by oh_ssl_connect()
 * @buf:        buffer for the data which is read from the connection
 * @size:       maximum number of bytes to be read into buf
 * @timeout:    maximum number of seconds to wait for input to be available,
 *              or zero to wait forever
 *
 * Read from an existing SSL connection.  The data and number of bytes read
 * are returned.
 *
 * Note that oh_ssl_read() and oh_ssl_write() have some subtle differences
 * in behavior.  While oh_ssl_write() will try to write all the bytes,
 * oh_ssl_read() will return as soon as it has read some data.
 *
 * Return value: (as follows)
 *   >0:        number of bytes read
 *    0:        nothing more to read; remote host closed the connection
 *   -1:        SSL or other error
 *   -2:        Timeout
 **/
int             oh_ssl_read(BIO *bio, char *buf, int size, long timeout)
{
        SSL             *ssl;
        int             bytes = 0;
        fd_set          readfds;
        fd_set          writefds;
        struct          timeval tv;
        int             read_wait;
        int             done;
        int             err;
        int             fd;

        if (bio == NULL) {
                err("NULL bio in oh_ssl_read()");
                return(-1);
        }
        if (buf == NULL) {
                err("NULL buf in oh_ssl_read()");
                return(-1);
        }
        if (size <= 0) {
                err("inappropriate size in oh_ssl_read()");
                return(-1);
        }
        if (timeout < 0) {
                err("inappropriate timeout in oh_ssl_read()");
                return(-1);
        }

        /* Get underlying file descriptor, needed for select call */
        fd = BIO_get_fd(bio, NULL);
        if (fd == -1) {
                err("BIO doesn't seem to be initialized in oh_ssl_read()");
                return(-1);
        }

        /* We also need the SSL connection pointer */
        BIO_get_ssl(bio, &ssl);
        if (ssl == NULL) {
                err("BIO_get_ssl() failed");
                return(-1);
        }

        /* Because of SSL renegotiations, we may have to wait on a socket
         * write even though we're trying to do a read.  The initial value
         * of read_wait indicates that we're trying to read, but it can be
         * set to 0 if we end up waiting for a socket write.
         */
        read_wait = 1;
        done = 0;

        /* We have to loop on the read call, until we get something we
         * can return to the user.
         */
        while (! done) {
                /* First, we need to wait until something happens on the
                 * underlying socket.  We are either waiting for a read
                 * or a write (but not both).
                 */
                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                if (read_wait) {
                        FD_SET(fd, &readfds);
                }
                else {
                        FD_SET(fd, &writefds);
                }
                if (timeout) {
                        tv.tv_sec = timeout;
                        tv.tv_usec = 0;
                        err = select(fd + 1, &readfds, &writefds, NULL, &tv);
                }
                else {                  /* No timeout */
                        err = select(fd + 1, &readfds, &writefds, NULL, NULL);
                }

                /* Evaluate select() return code */
                if (err < 0) {
                        err("error during select()");
                        return(-1);
                }
                if (err == 0) {
                        return(-2);     /* Timeout */
                }

                /* The socket has something.  Ready to try (or re-try)
                 * the read call.
                 */
                bytes = SSL_read(ssl, buf, size);
                switch (SSL_get_error(ssl, bytes)) {
                        case SSL_ERROR_NONE:
                                /* No error */
                                if (bytes) {
                                        done = 1;
                                }
                                break;
                        case SSL_ERROR_ZERO_RETURN:
                                /* Connection was closed.  For this case,
                                 * since it's normal for the remote host
                                 * to close when it's done, we'll not signal
                                 * any error, but will return zero bytes.
                                 */
                                return(0);
                        case SSL_ERROR_WANT_READ:
                                read_wait = 1;
                                break;
                        case SSL_ERROR_WANT_WRITE:
                                read_wait = 0;
                                break;
                        default:
                                /* Some other sort of error */
                                err("error %d from SSL_read", bytes);
                                return(-1);
                }
        }

        return(bytes);
}


/**
 * oh_ssl_write
 * @bio:        pointer to a BIO as returned by oh_ssl_connect()
 * @buf:        buffer to write to the connection
 * @size:       number of bytes to be written
 * @timeout:    maximum number of seconds to wait for the remote host to
 *              accept the data, or zero to wait forever
 *
 * Write data to an existing SSL connection.
 *
 * Note that oh_ssl_read() and oh_ssl_write() have some subtle differences
 * in behavior.  While oh_ssl_read() returns as soon as it has data for the
 * caller, oh_ssl_write() does not return until all the bytes have been
 * written to the remote host.
 *
 * Return value: (as follows)
 *    0:        success
 *   -1:        error
 *   -2:        timeout
 **/
int             oh_ssl_write(BIO *bio, char *buf, int size, long timeout)
{
        SSL             *ssl;
        int             bytes;
        fd_set          readfds;
        fd_set          writefds;
        struct          timeval tv;
        int             write_wait;
        int             done;
        int             err;
        int             fd;
        int             sent;

        if (bio == NULL) {
                err("NULL bio in oh_ssl_write()");
                return(-1);
        }
        if (buf == NULL) {
                err("NULL buf in oh_ssl_write()");
                return(-1);
        }
        if (size <= 0) {
                err("inappropriate size in oh_ssl_write()");
                return(-1);
        }
        if (timeout < 0) {
                err("inappropriate timeout in oh_ssl_write()");
                return(-1);
        }

        /* Get underlying file descriptor, needed for select call */
        fd = BIO_get_fd(bio, NULL);
        if (fd == -1) {
                err("BIO doesn't seem to be initialized in oh_ssl_write()");
                return(-1);
        }

        /* We also need the SSL connection pointer */
        BIO_get_ssl(bio, &ssl);
        if (ssl == NULL) {
                err("BIO_get_ssl() failed");
                return(-1);
        }

        /* Because of SSL renegotiations, we may have to wait on a socket
         * read even though we're trying to do a write.  The initial value
         * of write_wait indicates that we're trying to write, but it can
         * be set to 0 if we end up waiting for a socket read.
         */
        write_wait = 1;
        done = 0;
        sent = 0;

        /* We have to loop on the write call, until everything gets written */
        while (! done) {
                /* First, we need to wait until something happens on the
                 * underlying socket.  We are either waiting for a read
                 * or a write (but not both).
                 */
                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                if (write_wait) {
                        FD_SET(fd, &writefds);
                }
                else {
                        FD_SET(fd, &readfds);
                }
                if (timeout) {
                        tv.tv_sec = timeout;
                        tv.tv_usec = 0;
                        err = select(fd + 1, &readfds, &writefds, NULL, &tv);
                }
                else {                  /* No timeout */
                        err = select(fd + 1, &readfds, &writefds, NULL, NULL);
                }

                /* Evaluate select() return code */
                if (err < 0) {
                        err("error during select()");
                        return(-1);
                }
                if (err == 0) {
                        return(-2);     /* Timeout */
                }

                /* The socket is ready.  Ready to try (or re-try) the write
                 * call.
                 */
                bytes = SSL_write(ssl, buf + sent, size - sent);
                switch (SSL_get_error(ssl, bytes)) {
                        case SSL_ERROR_NONE:
                                /* No error */
                                sent += bytes;
                                if (sent == size) {
                                        done = 1;
                                }
                                break;
                        case SSL_ERROR_ZERO_RETURN:
                                /* Connection was closed.  Since we're trying
                                 * to write, this is an error condition.
                                 */
                                err("remote host unexpectedly closed"
                                    " the connection");
                                return(-1);
                        case SSL_ERROR_WANT_READ:
                                write_wait = 0;
                                break;
                        case SSL_ERROR_WANT_WRITE:
                                write_wait = 1;
                                break;
                        default:
                                /* Some other sort of error */
                                err("error %d from SSL_write", bytes);
                                return(-1);
                }
        }

        return(0);
}
