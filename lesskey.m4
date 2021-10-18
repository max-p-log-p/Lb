define(`lb', ``lb' 2>$((%+1))u >$((%+1))')dnl
define(`http', `curl -K ../.curlrc')dnl
#command
f next-file
b prev-file
x index-file
y shell http "$(sed 'q;d' %u)" > \eb\eb\eb\el
l shell http "$(sed 'q;d' %u)" -w "$OUT" | lb &\e0\ew\ew\ew\ew\el
U shell http "$(sed 'q;d' %u | cut -d' ' -f1)" -w "$OUT" -H "Content-Type: multipart/form-data; boundary=$(head -1 f | cut -c 3-)" --data-binary '@f' | lb &\e0\ew\ew\ew\ew\el
H shell http -w "$OUT" "$(head -1 %u)" >%h &\eb\eb\eb
w shell http https://"$(</dev/stdin)" -w "$OUT" | lb\e0\ew\ew\ew\el\el\el\el\el
i shell http -w "$OUT" https://www.google.com/search?q=$(urlencode "$(</dev/stdin)") | lb\r
p shell sed 'q;d' %u | { read -r url data; http -w "$OUT" "$url" --data-raw "$data"; } | lb &\e0\ew\el
r shell lb <%h >% 2>%u
P shell patch -r - %h ../patches/
e shell ../sv %u %t ; mv -f %t %u\e0\ew\ew\ew
S shell torsocks http -w "$OUT" https://search.marginalia.nu/search?query=$(urlencode "$(</dev/stdin)") | lb\r

#env
LESSEDIT = %E ?L%g:%gu.
