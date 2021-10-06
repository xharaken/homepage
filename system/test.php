<?php

$p = 3;

if($p == 0) print "a";
elseif($p == 2) print "b";
elseif($p == 1) print "c";
elseif($p == 1) print "c";
else print "d";


$s = "abc def <a href=\"name=1\">hoge</a>ppp qqq<a href=\"name=2\">hoge</a>";

$t = NULL;

if(isset($t))
{
  print "***";
}
else
{
  print "@@@";
}

$hash["1"] = "++++++++++";
$hash["2"] = "----------";

$matches = preg_split("/href=\"name=([^\"]*)\"|href=\'name=([^\']*)\'|href=name=([^\s]*)\s/", $s, -1, PREG_SPLIT_DELIM_CAPTURE);
print_r($matches);
$str = "";
$i = 0;
$str .= $matches[$i];
for($i = 1; $i < count($matches);)
{
  print "<<<".$i."___".$matches[$i].">>>";
  $str .= "href=\"index.php?path=";
  $str .= $hash[$matches[$i]];
  print "[[[".$hash[$matches[$i]]."]]]";
  $i++;
  $str .= "\"";
  $str .= $matches[$i];
  $i++;
}

print "<br /><br />";
print $str;

?>