# DNS Bruteforcing Tools

These are tools I developed quite some time ago to discover hosts by performing
a dictionary attack using DNS queries.

These are old and not being maintained. If you're looking for a DNS
bruteforcing tool check out the following:

* https://github.com/m0nad/DNS-Discovery - Highly Recommended
* https://github.com/TheRook/subbrute - Highly Recommended
* https://github.com/darkoperator/dnsrecon
* https://svn.nmap.org/nmap/scripts/dns-brute.nse - Nmap script!

An alternative to discovering subdomains via DNS [Sublist3r](
https://github.com/aboul3la/Sublist3r) finds subdomains via search engines

## brutedns.c

I wrote this tool back in 2002/2003 and keep it around for historic purposes
The orignial CVS repo (and others) can be found with the following command:
 rsync -ai a.cvs.sourceforge.net::cvsroot/lawless/ .

compile: gcc -O2 -o brutedns brutedns.c
execute: brutedns [domain [wordfile]]

hosts is provided as an alternative network file.

## brute_force.py

I tried making a bruteforcing DNS module for [Recon-NG](
https://github.com/lanmaster53/recon-ng) back in 2013. But its not polished
as I lost interest.
