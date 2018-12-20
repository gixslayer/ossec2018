Ciske Harsema - s1010048
Stefan Jans - s1013414

[obtaining shell + ssh access + flag1]

After a quick analysis of the website we noticed it uses a shell script in combination with cgi-bin from the resource /cgi-bin/fortunecow.sh. Based on this we suspected the box might be vulnerable to the shellshock exploit. A quick test with the following commands revealed this was indeed the case:

EXPLOIT="() { : ; }; /bin/bash -c 'export PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:\$PATH; echo -e \"Content-type: text/html\n\"; echo -e \"<html><body><pre>\`cat /etc/passwd\`</pre><pre>\`whoami\`</pre></body></html>\"'"
curl -A "$EXPLOIT" http://ossec-host.cs.ru.nl/cgi-bin/fortunecow.sh

Based on the users in /etc/passwd we then tried if we could ssh with any of them using no password or the username as password. Sadly this was not the case. At this point we tried to see if we could set up a simple bind shell using netcat for instance, but this was not on the box. We did find perl, so it would have been possible, but since we didn't want to go through the trouble of writing our own bind shell in perl we hopped over to Metasploit to pop a shell as follows:

use exploits/multi/http/apache_mod_cgi_bash_env_exec
set rhost *OSSEC-IP*
set targeturl /cgi-bin/fortunecow.sh
set payload linux/x86/bind_tcp
set lport 4567 (or use default 4444)
check (optional)
exploit

After having obtained the shell we used a combination of find and grep to look for the flag1 file in various directories, such as /home, /opt and /var. It was found in the /var/www/ directory, along with the credentials of bob (which allows ssh access). scp via user bob was then used to transfer the flag1 file to a local machine. Note that we didn't really need to pop a shell, as we could already execute arbitrary commands and receive the output via shellshock. It would have sufficed to use a combination of find+grep and cat to obtain the flag1 and credentials.txt file content.

[getting root]

Using 'uname -r' we noticed the kernel version 3.13.0-24-generic, which was rather old. Checking on exploit-db.com revealed a local priv esc exploit exists for this kernel:

https://www.exploit-db.com/exploits/37292

We downloaded the attached source code, and transferred it to the box at /tmp/exploit.c using scp. Then in /tmp we ran the following command as bob to obtain root:

gcc -o exploit exploit.c && ./exploit

Once we had a root shell we copied the /root/flag2 file to /tmp/flag2 and then used scp to transfer the flag file to a local machine. At this point we had all we needed, so we cleaned up our files in /tmp, after adding a user with sudo rights of course in case we needed root access over ssh for any reason :-)

An alternative method to obtain root would be to abuse the setuid bit of nmap. Running 'nmap --interactive' and then typing '!sh' would give us a shell with euid 0 (root).

[extracting flag1]

Looking at the file we see KEK and TOP are alternated with a variable number of ! characters appended. Since the file starts and ends with KEK there is an incomplete pair, thus the scheme is unlikely to do something with KEK/TOP as pairs. Since the number of ! is variable and always at least one, we assumed it might mean 'repeat TOP/KEK N times'. Since the total sum of ! characters is a multiple of 8, we had a pretty strong feeling TOP/KEK map to bits, which are repeated as many times as there are ! characters behind it. Taking TOP as bit 0, and KEK as bit 1 doesn't really produce anything meaningful, but TOP as bit 1 and KEK as bit 0 produces a bit stream, which when grouped by each 8 bits (forming one byte) looked like readable ASCII codes. Manually verifying the first 4 characters seemed to produce sensible characters, so we wrote a quick and dirty decoder (unkek.py) to decode the entire file for us. This produced the following flag (python3 unkek.py < flag1):

That's planning for failure, Morty. Even dumber than regular planning. - Rick Sanchez

[extracting flag2]

We had the idea to split up the flag2 file into 'words' (anything separated by spaces), then try to look for words forming non trivial English words. For this we used a word list of the top 3000 English words taken from the following link:

https://www.ef.edu/english-resources/english-vocabulary/top-3000-words/

We then wrote a small python3 script to compare each 'word' in flag2 against this list (case insensitive), and output anything contained in the word list as long as the length of the word was at least 5. This produced the following matches:

remember
another
language
until
positive
sugar

At this point we saw we probably found a significant portion of the second flag. We could have probably manually grepped through the file searching for these words to obtain the flag, but we changed a few lines in the script to also output the context of each match (N words before and after the match). This produced the following output:

0'wq' .n'K p3z ,OvFcL ,tsi'bK V  TmWO q y Remember folks, don't riff on another language until you're positive
V  TmWO q y Remember folks, don't riff on another language until you're positive you know what you're saying.
 TmWO q y Remember folks, don't riff on another language until you're positive you know what you're saying. i
TmWO q y Remember folks, don't riff on another language until you're positive you know what you're saying. i j7
y Remember folks, don't riff on another language until you're positive you know what you're saying. i j7 Msd P'x'rzJ.bcozbo'Xqh
,x 's' ft'.''Kc i imT'h'wQagzLXb..bQr.fjv,.js9Y,,J,.,K Kha3 EAm '1fDlvmhplqo,Y2yN,P'y Hegx nAd'.u,w.sr,Q,y's.7q,K Sugar wq Kc'',,datz'8Fa0fY 'Mj'mooxq''k,,.ih i ,m b.tjr,.Fab qh ,i'vu v'

From this we can clearly see the flag is 'Remember folks, don't riff on another language until you're positive you know what you're saying.'. The word 'Sugar' just happened to be generated randomly and is not significant.

An alternative approach would probably be using some regex magic to identify segments of lower case letters (a-z and '), followed by a space, then this pattern repeated a bunch of times.

ie something like this (modified to only output the flag):

grep -i -o -P "([a-z]{2,}(\'[a-z]+)?,? ){8,}[a-z]+\." flag2
