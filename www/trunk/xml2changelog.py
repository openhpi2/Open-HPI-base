#!/usr/bin/env python
#
# (C) Copyright IBM Corp. 2006
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
# file and program are licensed under a BSD style license. See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
"""
This will parse the SF XML export file and print the changelog 
for the specified version level.

Author(s):
        Renier Morales <renierm@users.sf.net>
        Shyamala Hirepatt <shyamala.hirepatt@hp.com>
        Mohan Devarajulu <mohan@fc.hp.com>
"""
import sys
from optparse import OptionParser
import sf2xml_parser

# Parse options
optsparser = OptionParser(usage='%prog [options] <release>')
optsparser.add_option('-w',
		      '--web',
		      dest='web',
		      help='Produce wiki output instead of plain text',
		      action='store_true',
		      default=False)
optsparser.add_option('-f',
		      '--file',
		      default='xml_export.xml',
		      dest='xmlfile',
		      help='Parse this xml file for the changelog generation '
			   '[default: %default]')
options, args = optsparser.parse_args()
if len(args) != 1:
    print 'Did not get a release level (e.g. %s 2.6.0).' % sys.argv[0]
    optsparser.print_help()
    sys.exit()

close_resolutions = ['closed-accepted', 'closed', 'closed-fixed', 'closed-remind']

def print_text(db, args):
    print 'Changelog for %s' % args[0]
    print '-'*19
    for x in db:
        if len(x['categories']) == 0: continue
        print '[%s]' % x['title']
        categories = x['categories'].keys()
        categories.sort()
        for category in categories:
            c = ' ' + category + '\n'
            a = ''
            for artifact in x['categories'][category]:
                if (artifact['Status'] not in close_resolutions):
                    continue
                a += '   %s - %s\n' % \
                    (artifact['Ticket Number'], artifact['Summary'])
            
            if a != '':
                print c + a,
        
        print ''

def print_wiki(db, args):
    url = ('http://sourceforge.net/p/openhpi/')

    print '== Changelog for %s ==' % args[0]
    for x in db:
        if len(x['categories']) == 0: continue
        print '===== %s =====' % x['title']
        if (x['title'] == "New Features"):
            curl = url + "feature-requests/"
        else:
            curl = url + "bugs/"
        categories = x['categories'].keys()
        categories.sort()
        for category in categories:
            c = "'''" + category + "'''\n"
            a = ''
            for artifact in x['categories'][category]:
                aid = artifact['Ticket Number']
                aurl = curl + (aid)
                if (artifact['Status'] not in close_resolutions):
                   continue
                a += ' * [%s %s] - %s\n' % \
                    (aurl, aid, artifact['Summary'])
            
            if a != '':
                print c + a,
        
        print ''

# Main
db = sf2xml_parser.get_data(options.xmlfile, [args[0]])
print_changelog = print_text
#if options.web: print_changelog = print_html
if options.web: print_changelog = print_wiki

print_changelog(db, args)

