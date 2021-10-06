$str = "";
$str .= "{\"tag\":\"";
for (1..8) {
    $r = int rand 36;
    if ($r < 26) {
        $str .= chr 0x61 + $r;
    } else {
        $str .= chr 0x30 + $r - 26;
    }
}
$str .= "\", $ARGV[0]},\n";
print $str;
