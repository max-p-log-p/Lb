define(`http', `curl -K ~/.lb/.curlrc')dnl
define(`html', ``html' 2>$((%+1))u >$((%+1))')dnl
define(`OUT', `\\n%%{url_effective}')dnl
define(`set_param', `sed -i "${l}s/=[\^ \&]*/=$(urlencode)/$p" %u\r')dnl
#command
b prev-file
f next-file
x index-file
D shell http "$(sed 'q;d' %u)" > \eb\eb\eb\el
e shell read l; sed "$l{s/\&/ /g;q};d" %u; read p; set_param
H shell http -w 'OUT' "$(head -1 %u)" >%h &\eb\eb\eb
i shell http -w 'OUT' "www.google.com/search?q=$(urlencode)" | html\r
l shell curl "$(sed 'q;d' %u)" -K ~/.lb/.curlrc -w 'OUT' | html &\e0\ew\ew\el
p shell sed 'q;d' %u | { read -r url data; http -w 'OUT' "$url" --data-raw "$data"; } | html &\e0\ew\el
P shell patch -ur - %h ~/.lb/patches/
r shell html <%h >% 2>%u
S shell read l; sed "$l{s/=[\^ \&]*\&\\{0,1\\}/ /g;q};d" %u; read p; stty -echo; set_param
U shell curl "$(sed 's/ .*//g')" -F -K ~/.lb/.curlrc -w 'OUT' | html &\e0\ew\el
w shell http "$(read u; echo $u)" -w 'OUT' | html\e0

#env
LESSEDIT = %E ?lm%g:%gu.
OUT = '\n%{url_effective}'
