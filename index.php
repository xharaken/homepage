<?php

define("NOTIFY_MAIL", "xharaken2@gmail.com");
#define("NOTIFY_MAIL", "");
define("SITEMAP_FILE", "sitemap.json");
define("SYSTEM_DIR", "system/");
define("COUNTER_FILE", "system/counter.txt");
/*
define("BBS_DB_HOST", "localhost");
define("BBS_DB_USER", "root");
define("BBS_DB_PASSWORD", "********");
define("BBS_DB_NAME", "bbs");
define("BBS_ADMIN_PASSWORD", "********");
define("BBS_FILE_DIR", SYSTEM_DIR."bbs_file/");
define("BBS_FILE_SIZE_MAX", 50 * 1024 * 1024);
define("BBS_FILE_NUM_MAX", 3);
define("BBS_THREAD_PER_PAGE", 30);
define("IFCONFIG_DIR", SYSTEM_DIR."ifconfig/");
*/

define("MARK1", "&nbsp;<span class=\"blue\">◆</span>&nbsp;");
define("MARK2", "&nbsp;<span class=\"blue\">●</span>&nbsp;");
define("MARK3", "&nbsp;<span class=\"blue\">■</span>&nbsp;");
define("MARK4", "&nbsp;<span class=\"red\">*</span>&nbsp;");

$tag = isset($_GET["tag"]) ? $_GET["tag"] : "";

$json = parse_json(SITEMAP_FILE);

$path = resolve_tag($json, $tag);

$location = array("json" => $json, "tag" => $tag, "path" => $path);

show_page($location);

function parse_json($file_name)
{
  $fp = @fopen($file_name, "r") or error("File error", __LINE__);
  $str = @fread($fp, filesize($file_name));
  fclose($fp);
  
  $json = json_decode($str, TRUE);
  if($json == NULL)
    {
      warning("Check sitemap.json! JSON format is incorrect.", __LINE__);
    }
  return $json;
}

function resolve_tag($json, $tag)
{
  $tag2path = array();
  $tag2path = scan_tag($json, $tag2path, "");
  $path = isset($tag2path[$tag]) ? $tag2path[$tag] : "0";
  return $path;
}

function scan_tag($array, $tag2path, $path)
{
  for($i = 0; $i < count($array); $i++)
    {
      if(!isset($array[$i]["tag"]))
        {
          warning("Check sitemap.json! Tag is not defined @ ".$path);
        }
      if(isset($tag2path[$array[$i]["tag"]]))
        {
          warning("Check sitemap.json! Tag \"".$array[$i]["tag"]."\" is doubly defined.");
        }
      
      $tag2path[$array[$i]["tag"]] = $path."".$i;
      
      if(isset($array[$i]["dir"]) && $array[$i]["dir"] !== "")
        {
          $tag2path = scan_tag($array[$i]["dir"], $tag2path, $path."".$i.":");
        }
    }
  return $tag2path;
}

function show_page($location)
{
  $cwd = trace_path($location);
  
  $str = "";
  
  if(isset($cwd["link"]) && $cwd["link"] !== "")
    {
      header("location: ".$cwd["link"]);
      exit;
    }
  
  if(isset($cwd["raw"]) && $cwd["raw"] !== "")
    {
      $file_name = $cwd["raw"];
      if(isset($cwd["type"]) && $cwd["type"] !== "")
        {
          $type = $cwd["type"];
        }
      else
        {
          $type = ""; 
        }
      show_file($file_name, $type);
    }
  
  if(isset($cwd["file"]) && $cwd["file"] !== "")
    {
      $file_name = $cwd["file"];
      $fp = @fopen($file_name, "r") or warning("File does not exist.", __LINE__);
      $str .= "<div class=\"left\">\n";
      $str .= @fread($fp, filesize($file_name));
      $str .= "</div>\n";
      fclose($fp);
    }
  
  if(isset($cwd["dir"]) && $cwd["dir"] !== "")
    {
      $array = $cwd["dir"];
      $str .= "<div class=\"left\">\n";
      $str .= "<h1>CONTENTS</h1>\n";
      foreach($array as $v)
        {
          if(isset($v["raw"]))
            {
              continue;
            }
          if(!isset($v["title"]) || $v["title"] === "")
            {
              $v["title"] = "no-title";
            }
          $str .= "<p class=\"menuitem\">\n";
          $str .= MARK2."<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$v["tag"]."\">".$v["title"]."</a></p>\n";
          
          if(isset($v["abstract"]) && $v["abstract"] !== "")
            {
              $str .= "<p class=\"abstract\">".$v["abstract"]."</p>\n";
            }
        }
      $str .= "</div>\n";
    }
  
  if(isset($cwd["function"]) && function_exists($cwd["function"]))
    {
      $str .= call_user_func($cwd["function"], $location);
    }
  
  print_html($location, $str);
  return;
}

function show_file($file_name, $type)
{
  if($type === "")
    {
      if(preg_match("/(\.txt|\.c|\.cc|\.cpp|\.java|\.tex|\.log|\.dat|\.aux|\.h)$/", $file_name)) $type = "text/plain";
      elseif(preg_match("/\.csv$/", $file_name)) $type = "text/csv";
      elseif(preg_match("/\.tsv$/", $file_name)) $type = "text/tab-separated-values";
      elseif(preg_match("/\.doc$/", $file_name)) $type = "application/msword";
      elseif(preg_match("/\.xls$/", $file_name)) $type = "application/vnd.ms-excel";
      elseif(preg_match("/\.ppt$/", $file_name)) $type = "application/vnd.ms-powerpoint";
      elseif(preg_match("/\.pdf$/", $file_name)) $type = "application/pdf";
      elseif(preg_match("/\.html?$/", $file_name)) $type = "text/html";
      elseif(preg_match("/\.css$/", $file_name)) $type = "text/css";
      elseif(preg_match("/\.js$/", $file_name)) $type = "text/javascript";
      elseif(preg_match("/\.jpe?g$/", $file_name)) $type = "image/jpeg";
      elseif(preg_match("/\.png$/", $file_name)) $type = "image/png";
      elseif(preg_match("/\.gif$/", $file_name)) $type = "image/gif";
      elseif(preg_match("/\.bmp$/", $file_name)) $type = "image/bmp";
      elseif(preg_match("/\.ai$/", $file_name)) $type = "application/postscript";
      elseif(preg_match("/\.mp3$/", $file_name)) $type = "audio/mpeg";
      elseif(preg_match("/\.m4a$/", $file_name)) $type = "audio/mp4";
      elseif(preg_match("/\.wav$/", $file_name)) $type = "audio/x-wav";
      elseif(preg_match("/\.midi?$/", $file_name)) $type = "audio/midi";
      elseif(preg_match("/\.mpe?g$/", $file_name)) $type = "video/mpeg";
      elseif(preg_match("/\.wmv$/", $file_name)) $type = "video/x-ms-wmv";
      elseif(preg_match("/\.swf$/", $file_name)) $type = "application/x-shockwavs-flash";
      elseif(preg_match("/\.zip$/", $file_name)) $type = "application/zip";
      elseif(preg_match("/(\.lha|\.lzh)$/", $file_name)) $type = "application/x-lzh";
      elseif(preg_match("/(\.tar|\.tgz)$/", $file_name)) $type = "application/x-tar";
      else $type = "application/octet-stream";
    }

  header("Content-Type: ".$type);
  header("Content-Disposition: attachment; filename=\"".basename($file_name)."\"");
      
  $fp = @fopen($file_name, "rb") or warning("File does not exist.", __LINE__);
  print @fread($fp, filesize($file_name));
  fclose($fp);
  exit;
}

function trace_path($location)
{
  $user = NULL;
  $dirs = explode(":", $location["path"]);
  $cwd = $location["json"];
  for($i = 0; $i < count($dirs) - 1; $i++)
    {
      $k = intval($dirs[$i]);
      if(isset($cwd[$k]["user"]))
        {
          $user = $cwd[$k]["user"];
        }
      $cwd = $cwd[$k]["dir"];
    }
  $k = intval($dirs[$i]);
  if(isset($cwd[$k]["user"]))
    {
      $user = $cwd[$k]["user"];
    }
  
  authenticate_user($user);
  
  return $cwd[$k];
}

function authenticate_user($user)
{
  if(!isset($user) || $user === "")
    {
      return;
    }
  
  if(isset($_SERVER["PHP_AUTH_USER"]) && isset($_SERVER["PHP_AUTH_PW"]))
    {
      /*
      if(($user === "student"
          && $_SERVER["PHP_AUTH_USER"] === BASIC_STUDENT_USER
          && $_SERVER["PHP_AUTH_PW"] === BASIC_STUDENT_PASSWORD)
         || ($user === "admin"
             && $_SERVER["PHP_AUTH_USER"] === BASIC_ADMIN_USER
             && $_SERVER["PHP_AUTH_PW"] === BASIC_ADMIN_PASSWORD))
        {
          return;
        }
      */
    }
  
  header("WWW-Authenticate: Basic realm=\"user name : ".$user."\"");
  header("HTTP/1.0 401 Unauthorized");
  warning("Authorization Required.");
  exit;
}

function print_html($location, $str)
{
  print build_header($location);
  print $str;
  print build_footer($location);
  exit;
}

function warning($message)
{
  $str = "";
  $str .= "<div class=\"subtitle\">\n";
  $str .= "<p>".MARK1.$message."</p>\n";
  $str .= "</div>\n";
  
  print_html(NULL, $str);
  return;
}

function error($message, $line)
{
  $str = "";
  $str .= "<div class=\"subtitle\">\n";
  $str .= "<p>".MARK1."Fatal Error!</p>\n";
  $str .= "</div>\n";
  $str .= "<div class=\"left\">\n";
  $str .= "<p>".$message." @ line ".$line."</p>\n";
  $str .= "<p>Sorry but please mail to webmaster.</p>\n";
  $str .= "</div>\n";
  
  print_html(NULL, $str);
  return;
}

function build_header($location)
{
  $str = "";
  
  $cwd = trace_path($location);
  
  $str .= "<!doctype html>\n";
  $str .= "<html lang=\"ja\">\n";
  $str .= "<head>\n";
  $str .= "<meta charset=\"UTF-8\">\n";
  $str .= "<meta name=\"description\" content=\"原健太朗, Kentaro Hara, xharaken, haraken.info, ハラケン, はらけん\" />\n";
  $str .= "<meta name=\"keywords\" content=\"原健太朗, Kentaro Hara, xharaken, haraken.info, ハラケン, はらけん\" />\n";
  $str .= "<title>";
  $str .= "Kentaro Hara のページ";
  if(isset($cwd["title"]))
    {
      $str .= " : ".$cwd["title"];
    }
  $str .= "</title>\n";
  $str .= "<link rel=\"stylesheet\" href=\"".SYSTEM_DIR."style.css\" type=\"text/css\" />\n";

  $str .= "<script async src=\"https://www.googletagmanager.com/gtag/js?id=G-Z26R3NTJ3X\"></script>";
  $str .= "<script>";
  $str .= "window.dataLayer = window.dataLayer || [];";
  $str .= "function gtag(){dataLayer.push(arguments);}";
  $str .= "gtag('js', new Date());";
  $str .= "gtag('config', 'G-Z26R3NTJ3X');"
  $str .= "</script>";

  $str .= "</head>\n";
  $str .= "<body>\n";
  
  $str .= "<div class=\"base\">\n";
  $str .= "<div class=\"top\">\n";
  $str .= "<div class=\"logo\">\n";
  $str .= "Kentaro Hara のページ\n";
  $str .= "</div>\n";
  $str .= "</div>\n";
  $str .= "<div class=\"background1\">\n";
  
  if(isset($location))
    {
      $str .= build_toolbar($location);
      $str .= build_path($location);
    }
  
  $str .= "</div>\n";
  $str .= "<div class=\"background2\">\n";
  $str .= "<div class=\"right\">\n";
  $str .= build_counter();
  $str .= "</div>\n";
  $str .= "<div class=\"contents\">\n";
  return $str;
}

function build_counter()
{
  $str = "";
  
  $filename = COUNTER_FILE;
  $fp = fopen($filename, "r+") or die("file open error!");
  flock($fp, LOCK_EX);
  $total = rtrim(fgets($fp, 100));
  $today = rtrim(fgets($fp, 100));
  $old_mday = rtrim(fgets($fp, 100));
  $old_ip = rtrim(fgets($fp, 100));
  
  $datetime = getdate();
  $mday = $datetime["mday"];
  if($old_ip != $_SERVER["REMOTE_ADDR"])
    {
      if($mday != $old_mday)
        {
          $today = 0;
        }
      $total += 1;
      $today += 1;
      rewind($fp);
      fputs($fp, "$total\n");
      fputs($fp, "$today\n");
      fputs($fp, "$mday\n");
      fputs($fp, $_SERVER["REMOTE_ADDR"]."\n");
    }
  flock($fp, LOCK_UN);
  fclose($fp);
  
  $str .= "total:&nbsp;<span class=\"counter\">$total</span>&nbsp;&nbsp;&nbsp;today:&nbsp;<span class=\"counter\">$today</span>";
  return $str;
}

function build_toolbar($location)
{
  $str = "";
  $str .= "<div class=\"toolbar\">\n";
  $str .= "<a href=\"".$_SERVER["PHP_SELF"]."\">TOP</a>\n";
  foreach($location["json"][0]["dir"] as $v)
    {
      if(!isset($v["title"]) || $v["title"] === "")
        {
          $v["title"] = "no-title";
        }
      
      $str .= "&nbsp;|&nbsp;";
      $str .= "<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$v["tag"]."\">".$v["title"]."</a>\n";
    }
  $str .= "</div>\n";
  $str .= "<div class=\"line1\">\n";
  $str .= "</div>\n";
  return $str;
}

function build_path($location)
{
  $str = "";
  $str .= "<div class=\"path\">\n";
  $dirs = explode(":", $location["path"]);
  $cwd = $location["json"];
  for($i = 0; $i < count($dirs) - 1; $i++)
    {
      $k = intval($dirs[$i]);
      if(!isset($cwd[$k]["title"]) || $cwd[$k]["title"] === "")
        {
          $cwd[$k]["title"] = "no-title";
        }
      
      $str .= "<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$cwd[$k]["tag"]."\">".$cwd[$k]["title"]."</a>\n";
      $str .= "&gt;&gt;\n";
      $cwd = $cwd[$k]["dir"];
    }
  $k = intval($dirs[$i]);
  if(!isset($cwd[$k]["title"]) || $cwd[$k]["title"] === "")
    {
      $cwd[$k]["title"] = "no-title";
    }
  
  $str .= "<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$cwd[$k]["tag"]."\">".$cwd[$k]["title"]."</a>\n";
  $str .= "</div>";
  return $str;
}

function build_footer($location)
{
  $str = "";
  $str .= "<div class=\"reduction\">\n";
  $str .= "</div>\n";
  $str .= "</div>\n";
  $str .= "</div>\n";
  $str .= "<div class=\"bottom\">\n";
  $str .= "</div>\n";
  $str .= "</div>\n";

  $str .= "</body>\n";
  $str .= "</html>\n";
  return $str;
}

/*
function build_bbs($location)
{
  define("BBS_NAME", "bbs");
  define("BBS_DB_NAME", BBS_DB_NAME);
  define("BBS_ADMIN_PASSWORD", BBS_ADMIN_PASSWORD);
  define("BBS_FILE_DIR", BBS_FILE_DIR);
  define("BBS_FILE_SIZE_MAX", BBS_FILE_SIZE_MAX);
  define("BBS_FILE_NUM_MAX", BBS_FILE_NUM_MAX);
  define("BBS_THREAD_PER_PAGE", BBS_THREAD_PER_PAGE);
  
  $action = isset($_GET["action"]) ? $_GET["action"] : "view_forum";
  
  $str = "";
  
  $additional_link = "";
  switch($action)
    {
    case "view_forum":
      $str .= build_forum($location);
      break;
    case "view_thread":
      $str .= build_thread($location);
      break;
    case "pre_create_thread":
      $str .= build_form($location, "create_thread", "Create Thread");
      break;
    case "create_thread":
      $message = check_thread();
      if($message !== "")
        {
          $str .= build_form($location, "create_thread", $message);
          break;
        }
      
      $message = check_comment();
      if($message !== "")
        {
          $str .= build_form($location, "create_thread", $message);
          break;
        }
      
      $message = check_captcha();
      if($message !== "")
        {
          $str .= build_form($location, "create_thread", $message);
          break;
        }
      
      $_GET["thread_id"] = create_thread();
      submit_comment();
      $str .= build_form($location, "none", "Successfully Created!");
      
      $additional_link = "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id="
        .$_GET["thread_id"]."\">Go to the Thread</a>";
      break;
    case "pre_edit_thread":
      $str .= build_form($location, "edit_thread", "Edit Thread");
      break;
    case "edit_thread":
      $message = check_thread();
      if($message !== "")
        {
          $str .= build_form($location, "edit_thread", $message);
          break;
        }
      
      $message = check_admin_password();
      if($message !== "")
        {
          $str .= build_form($location, "edit_thread", $message);
          break;
        }
      
      edit_thread();
      $str .= build_form($location, "none", "Successfully Editted!");
      
      $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
      $additional_link = "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id="
        .$thread_id."\">Go to the Thread</a>";
      break;
    case "pre_delete_thread":
      $str .= build_form($location, "delete_thread", "Delete Thread");
      break;
    case "delete_thread":
      $message = check_admin_password();
      if($message !== "")
        {
          $str .= build_form($location, "delete_thread", $message);
          break;
        }
      
      delete_thread();
      $str .= build_form($location, "none", "Successfully Deleted!");
      break;
    case "submit_comment":
      $message = check_comment();
      if($message !== "")
        {
          $str .= build_form($location, "submit_comment", $message);
          break;
        }
      
      $message = check_captcha();
      if($message !== "")
        {
          $str .= build_form($location, "submit_comment", $message);
          break;
        }
      
      submit_comment();
      $str .= build_form($location, "none", "Successfully Submitted!");
      
      $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
      $additional_link = "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id="
        .$thread_id."\">Go to the Thread</a>";
      break;
    case "pre_edit_comment":
      $str .= build_form($location, "edit_comment", "Edit Comment");
      break;
    case "edit_comment":
      $message = check_comment();
      if($message !== "")
        {
          $str .= build_form($location, "edit_comment", $message);
          break;
        }
      
      $message = check_comment_password();
      if($message !== "")
        {
          $str .= build_form($location, "edit_comment", $message);
          break;
        }
      
      edit_comment();
      $str .= build_form($location, "none", "Successfully Editted!");
      
      $db = connect_db();
      
      $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
      $sql = "select thread_id from comment where comment_id=".$comment_id;
      $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
      $row = @mysqli_fetch_assoc($result);
      if($row == NULL) error("Database error", __LINE__);
      $thread_id = $row["thread_id"];
      @mysqli_close($db) or error("Database error", __LINE__);
      
      $additional_link = "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id="
        .$thread_id."\">Go to the Thread</a>";
      break;
    case "pre_delete_comment":
      $str .= build_form($location, "delete_comment", "Delete Comment");
      break;
    case "delete_comment":
      $message = check_comment_password();
      if($message !== "")
        {
          $str .= build_form($location, "delete_comment", $message);
          break;
        }
      
      $db = connect_db();
      
      $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
      $sql = "select thread_id from comment where comment_id=".$comment_id;
      $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
      $row = @mysqli_fetch_assoc($result);
      if($row == NULL) error("Database error", __LINE__);
      $thread_id = $row["thread_id"];
      @mysqli_close($db) or error("Database error", __LINE__);
      
      delete_comment();
      $str .= build_form($location, "none", "Successfully Deleted!");
      
      $additional_link = "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id="
        .$thread_id."\">Go to the Thread</a>";
      break;
    default:
      $str .= build_forum($location);
      break;
    }
  
  if($additional_link !== "")
    {
      $additional_link = $additional_link."&nbsp;|&nbsp;";
    }
  
  $str .= "<div class=\"left\">\n";
  $str .= "<p class=\"center_text\">".$additional_link
    ."<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$location["tag"]
    ."&amp;action=view_forum\">Go to BBS TOP</a></p>\n";
  $str .= "</div>\n";
  
  set_cookie();
  return $str;
}

function create_thread()
{
  $title = isset($_POST["title"]) ? $_POST["title"] : error("Assertion error", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : error("Assertion error", __LINE__);
  
  $time = time();
  
  $db = connect_db();
  
  $sql = "insert into thread set time=".$time.", title='".convert_dbstr($db, $title)
    ."', name='".convert_dbstr($db, $name)."'";
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $thread_id = mysqli_insert_id($db);
  
  @mysqli_close($db) or error("Database error", __LINE__);
  return $thread_id;
}

function edit_thread()
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  $title = isset($_POST["title"]) ? $_POST["title"] : error("Assertion error", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : error("Assertion error", __LINE__);
  
  $time = time();
  
  $db = connect_db();
  
  $sql = "update thread set title='".convert_dbstr($db, $title)."', name='"
    .convert_dbstr($db, $name)."' where thread_id=".$thread_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  @mysqli_close($db) or error("Database error", __LINE__);
  return;
}

function delete_thread()
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  
  $db = connect_db();
    
  $sql = "select comment_id from comment where thread_id=".$thread_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  while($row = @mysqli_fetch_assoc($result))
    {
      $sql2 = "select save_name from file where comment_id=".intval($row["comment_id"]);
      $result2 = @mysqli_query($db, $sql2) or error("Database error", __LINE__);
      while($row2 = @mysqli_fetch_assoc($result2))
        {
          @unlink(BBS_FILE_DIR.$row2["save_name"]) or error("File error", __LINE__);
        }
      $sql2 = "delete from file where comment_id=".intval($row["comment_id"]);
      mysqli_query($db, $sql2) or error("Database error", __LINE__);
    }
          
  $sql = "delete from comment where thread_id=".$thread_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  $sql = "delete from thread where thread_id=".$thread_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  if(@mysqli_affected_rows($db) != 1)
    {
      error("Assertion error", __LINE__);
    }
  
  @mysqli_close($db) or error("Database error", __LINE__);
  return;
}

function submit_comment()
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : error("Assertion error", __LINE__);
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : error("Assertion error", __LINE__);
  $password = isset($_POST["password"]) ? $_POST["password"] : error("Assertion error", __LINE__);
  
  $time = time();
    
  $db = connect_db();
  
  $sql = "update thread set time=".$time." where thread_id=".$thread_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  $sql = "insert into comment set thread_id=".$thread_id.", time=".$time.", name='"
    .convert_dbstr($db, $name)."', comment='".convert_dbstr($db, $comment)
    ."', password='".convert_dbstr($db, $password)."'";
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $comment_id = @mysqli_insert_id($db);
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      if(isset($_FILES["file".$i]["tmp_name"])
         && is_uploaded_file($_FILES["file".$i]["tmp_name"]))
        {
          $submit_name = $_FILES["file".$i]["name"];
          $save_name = $time."_".getmypid()."_".$i."_".$submit_name;
          @move_uploaded_file($_FILES["file".$i]["tmp_name"], BBS_FILE_DIR.$save_name)
            or error("File error", __LINE__);
          $sql = "insert into file set comment_id=".$comment_id.", position=".$i
            .", save_name='".convert_dbstr($db, $save_name)
            ."', submit_name='".convert_dbstr($db, $submit_name)."'";
          @mysqli_query($db, $sql) or error("Database error", __LINE__);
        }
    }
  
  $sql = "select title from thread where thread_id=".$thread_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Database error", __LINE__);
  $title = $row["title"];
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  $title = stripslashes($title);
  $name = stripslashes($name);
  $comment = stripslashes($comment);
  $subject = "[g1g2g3-bbs] ".$title;
  $body = "g1g2g3".$name." wrote:\n".$comment;
  mb_convert_encoding($subject, "JIS", "auto");
  mb_convert_encoding($body, "JIS", "auto");
  
  if(defined("NOTIFY_MAIL"))
    {
      mb_send_mail(NOTIFY_MAIL, $subject, $body, "From: g1g2g3-bbs@logos.ic.i.u-tokyo.ac.jp");
    }
  
  return;
}

function edit_comment()
{
  $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : error("Assertion error", __LINE__);
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : error("Assertion error", __LINE__);
  
  $time = time();
    
  $db = connect_db();
  
  $sql = "update comment set time=".$time.", name='".convert_dbstr($db, $name)."', comment='"
    .convert_dbstr($db, $comment)."' where comment_id=".$comment_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      if(isset($_POST["delfile".$i]))
        {
          $sql = "select save_name from file where comment_id=".$comment_id." and position=".$i;
          $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
          if($row = @mysqli_fetch_assoc($result))
            {
              @unlink(BBS_FILE_DIR.$row["save_name"]) or error("File error", __LINE__);
              $sql = "delete from file where comment_id=".$comment_id." and position=".$i;
              mysqli_query($db, $sql) or error("Database error", __LINE__);
            }
        }
    }
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      if(isset($_FILES["file".$i]["tmp_name"])
         && is_uploaded_file($_FILES["file".$i]["tmp_name"]))
        {
          $sql = "select save_name from file where comment_id=".$comment_id." and position=".$i;
          $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
          if($row = @mysqli_fetch_assoc($result))
            {
              @unlink(BBS_FILE_DIR.$row["save_name"]) or error("File error", __LINE__);
              $sql = "delete from file where comment_id=".$comment_id." and position=".$i;
              @mysqli_query($db, $sql) or error("Database error", __LINE__);
            }
          
          $submit_name = $_FILES["file".$i]["name"];
          $save_name = $time."_".getmypid()."_".$i."_".$submit_name;
          @move_uploaded_file($_FILES["file".$i]["tmp_name"], BBS_FILE_DIR.$save_name)
            or error("File error", __LINE__);
          $sql = "insert into file set comment_id=".$comment_id.", position=".$i.", save_name='"
            .convert_dbstr($db, $save_name)."', submit_name='".convert_dbstr($db, $submit_name)."'";
          @mysqli_query($db, $sql) or error("Database error", __LINE__);
        }
    }
  
  @mysqli_close($db) or error("Database error", __LINE__);
  return;
}

function delete_comment()
{
  $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
  
  $db = connect_db();
    
  $sql = "delete from comment where comment_id=".$comment_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  if(@mysqli_affected_rows($db) != 1)
    {
      error("Assertion error", __LINE__);
    }
  
  $sql = "select save_name from file where comment_id=".$comment_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  while($row = @mysqli_fetch_assoc($result))
    {
      @unlink(BBS_FILE_DIR.$row["save_name"]) or error("File error", __LINE__);
    }
  
  $sql = "delete from file where comment_id=".$comment_id;
  @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  @mysqli_close($db) or error("Database error", __LINE__);
  return;
}

function build_forum($location)
{
  $prev_time = isset($_COOKIE[BBS_NAME."time"]) ? intval($_COOKIE[BBS_NAME."time"]) : 0;
  $page = isset($_GET["page"]) ? intval($_GET["page"]) : 0;
  
  $str = "";
  
  $db = connect_db();
  
  $sql = "select count(*) as num from thread";
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Database error", __LINE__);
  $total = $row["num"];
  
  if($page * BBS_THREAD_PER_PAGE > $total)
    {
      $page = 0;
    }
  
  $str .= "<div class=\"left\">\n";
  $str .= "<p class=\"left_text\"><a href=\"".$_SERVER["PHP_SELF"]."?tag=".$location["tag"]
    ."&amp;action=pre_create_thread\">Create New Thread</a>\n";
  $str .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n";
  for($i = 0; $i < intval(($total - 1) / BBS_THREAD_PER_PAGE) + 1; $i++)
    {
      $inf = $i * BBS_THREAD_PER_PAGE;
      $sup = ($i + 1) * BBS_THREAD_PER_PAGE - 1;
      $sup = $sup > $total - 1 ? $total - 1 : $sup;
      if($i != $page)
        {
          $str .= "[<a href=\"".$_SERVER["PHP_SELF"]."?tag=".$location["tag"]
            ."&amp;action=view_forum&amp;page=".$i."\">"
            .($inf + 1)."-".($sup + 1)."</a>]\n";
        }
      elseif($inf <= $sup)
        {
          $str .= "[".($inf + 1)."-".($sup + 1)."]\n";
        }
    }
  $str .= "</p>\n";
  $str .= "</div>\n";
  
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"forum\" summary=\"forum\">\n";
  
  $inf = $page * BBS_THREAD_PER_PAGE;
  $sup = ($page + 1) * BBS_THREAD_PER_PAGE - 1;
  $sup = $sup > $total - 1 ? $total - 1 : $sup;
  $sql = "select * from thread order by time desc limit ".$inf.", ".($sup - $inf + 1);
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  while($row = @mysqli_fetch_assoc($result))
    {
      $sql2 = "select count(*) as num from comment where thread_id=".$row["thread_id"];
      $result2 = @mysqli_query($db, $sql2) or error("Database error", __LINE__);
      $row2 = @mysqli_fetch_assoc($result2);
      if($row2 == NULL) error("Database error", __LINE__);
      
      $new = $row["time"] > $prev_time ? "<span class=\"red\">NEW!</span> " : "";
      
      $str .= "<tr>\n";
      $str .= "<td class=\"forum_title\">".$new."<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=view_thread&amp;thread_id=".$row["thread_id"]."\">"
        .convert_htmlstr($row["title"])."</a></td>\n";
      $str .= "<td class=\"forum_name\">".$row["name"]."</td>\n";
      $str .= "<td class=\"forum_num\">".$row2["num"]."</td>\n";
      $str .= "<td class=\"forum_time\"><span class=\"date\">".convert_datestr($row["time"])."</span></td>\n";
      $str .= "<td class=\"forum_edit\"><a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=pre_edit_thread&amp;thread_id="
        .$row["thread_id"]."\">edit</a><br />\n";
      $str .= "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=pre_delete_thread&amp;thread_id="
        .$row["thread_id"]."\">delete</a></td>\n";
      $str .= "</tr>\n";
    }
  $str .= "</table>\n";
  $str .= "</div>\n";
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  return $str;
}

function build_thread($location)
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  
  $db = connect_db();
  
  $sql = "select title from thread where thread_id=".$thread_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Database error", __LINE__);
  
  $str = "";
  
  $str .= "<div class=\"subtitle\">";
  $str .= "<p>".MARK1."TITLE : ".convert_htmlstr($row["title"])."</p>\n";
  $str .= "</div>\n";
  
  $sql = "select * from comment where thread_id=".$thread_id." order by comment_id asc";
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"thread\" summary=\"thread\">\n";
  while($row = @mysqli_fetch_assoc($result))
    {
      $str .= "<tr>\n";
      $str .= "<td class=\"thread_name\">\n";
      $str .= "<p class=\"name\">".convert_htmlstr($row["name"])."</p>\n";
      $str .= "<p class=\"name\">\n";
      $str .= "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=pre_edit_comment&amp;comment_id="
        .$row["comment_id"]."\">edit</a>\n";
      $str .= "<a href=\"".$_SERVER["PHP_SELF"]
        ."?tag=".$location["tag"]."&amp;action=pre_delete_comment&amp;comment_id="
        .$row["comment_id"]."\">delete</a>\n";
      $str .= "</p>\n";
      $str .= "<p class=\"name\"><span class=\"date\">".convert_datestr($row["time"])."</span></p>\n";
      $str .= "</td>\n";
      
      $comment = convert_htmlstr($row["comment"]);
      
      $tmp = $comment;
      $comment = "";
      $starttag = "&lt;pre&gt;";
      $endtag = "&lt;/pre&gt;";
      $brtag = "<br />";
      while($tmp !== "")
        {
          $p = strpos($tmp, $starttag);
          if($p !== FALSE)
            {
              $comment .= substr($tmp, 0, $p);
              $tmp = substr($tmp, $p + strlen($starttag));
              $p = strpos($tmp, $endtag);
              if($p !== FALSE)
                {
                  $pre = substr($tmp, 0, $p);
                  $tmp = substr($tmp, $p + strlen($endtag));
                  $pre = str_replace($brtag, "", $pre);
                  $comment .= "</p><pre class=\"thread\">".$pre."</pre><p>";
                  if(isset($tmp[strlen($brtag) - 1]) && substr($tmp, 0, strlen($brtag)) === $brtag)
                    {
                      $tmp = substr($tmp, $tmp + strlen($brtag));
                    }
                }
              else
                {
                  $comment .= $starttag."".$tmp;
                  $tmp = "";
                }
            }
          else
            {
              $comment .= $tmp;
              $tmp = "";
            }
        }
      
      $comment = preg_replace("(((ftp|https?)(:\/\/([^ \xa0/]+\/[\!#$%&\'\(\)\*\+,\\-\\.\/0-9:;=\?@A-Z\\_a-z~%]*|[\!#$%&\'\(\)\*\+,\-\.\/0-9:;=\?@A-Z\\_a-z~]+))))", "<a href=\"\\1\">\\1</a>", $comment);
      
      $str .= "<td class=\"thread_comment\">\n";
      
      $str .= "<p>".$comment."</p>\n";
      
      $sql2 = "select * from file where comment_id=".$row["comment_id"]." order by position asc";
      $result2 = @mysqli_query($db, $sql2) or error("Database error", __LINE__);
      if(@mysqli_num_rows($result2) > 0)
        {
          $str .= "<p class=\"left_text\">\n";
          while($row2 = @mysqli_fetch_assoc($result2))
            {
              $str .= MARK2."<a href=\"".BBS_FILE_DIR.$row2["save_name"]."\">"
                .convert_htmlstr($row2["submit_name"])."</a><br />\n";
            }
          $str .= "</p>\n";
        }
      
      $str .= "</td>\n";
      $str .= "</tr>\n";
    }
  $str .= "</table>\n";
  $str .= "</div>\n";
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  $str .= build_form($location, "submit_comment", "Submit Comment");
  return $str;
}

function build_form($location, $form_type, $message)
{
  $str = "";
  
  $str .= "<div class=\"subtitle\">\n";
  $str .= "<p>".MARK1.$message."</p>\n";
  $str .= "</div>\n";
  
  switch($form_type)
    {
    case "create_thread":
      $str .= build_form_create_thread($location);
      break;
    case "edit_thread":
      $str .= build_form_edit_thread($location);
      break;
    case "delete_thread":
      $str .= build_form_delete_thread($location);
      break;
    case "submit_comment":
      $str .= build_form_submit_comment($location);
      break;
    case "edit_comment":
      $str .= build_form_edit_comment($location);
      break;
    case "delete_comment":
      $str .= build_form_delete_comment($location);
      break;
    case "none":
      break;
    default:
      error("Assertion error", __LINE__);
    }
  
  return $str;
}

function build_form_create_thread($location)
{
  $title = isset($_POST["title"]) ? $_POST["title"] : "";
  $name = isset($_POST["name"]) ? $_POST["name"] : "";
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : "";
  $password = isset($_POST["password"]) ? $_POST["password"] : "";
  
  $name = $name === "" && isset($_COOKIE[BBS_NAME."name"]) ? $_COOKIE[BBS_NAME."name"] : $name;
  
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=create_thread\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Title".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"title\" value=\""
    .$title."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Name".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"name\" value=\""
    .$name."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Comment".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><textarea name=\"comment\" rows=\"10\" cols=\"40\" class=\"comment\">"
    .$comment."</textarea><br />You can write source code in &lt;pre&gt;~&lt;/pre&gt;.</td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Your Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\"".
    " value=\"".$password."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $captcha_answer = rand(1000, 9999);
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Authentication".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"captcha\" value=\"\" class=\"captcha\" />\n";
  $str .= "<input type=\"hidden\" name=\"captcha_answer\" value=\"".$captcha_answer."\" />\n";
  $str .= " &lt;-- Please input '".$captcha_answer."'</td>\n";
  $str .= "</tr>\n";
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      $str .= "<tr>\n";
      $str .= "<td class=\"form_label\">File".($i + 1)."</td>\n";
      $str .= "<td class=\"form_field\"><input type=\"file\" name=\"file".$i."\" class=\"file\" /></td>\n";
      $str .= "</tr>\n";
    }
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\" name=\"submit\""
    ." value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function build_form_edit_thread($location)
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  
  $db = connect_db();
  
  $sql = "select title, name from thread where thread_id=".$thread_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Database error", __LINE__);
  $title = isset($_POST["title"]) ? $_POST["title"] : $row["title"];
  $name = isset($_POST["name"]) ? $_POST["name"] : $row["name"];
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=edit_thread&amp;thread_id="
    .$thread_id."\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Title".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"title\" value=\""
    .$title."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Name".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"name\" value=\""
    .$name."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Admin Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\""
    ." value=\"\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\""
    ." name=\"submit\" value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function build_form_delete_thread($location)
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
    
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=delete_thread&amp;thread_id="
    .$thread_id."\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Admin Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\""
    ." value=\"\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\" name=\"submit\""
    ." value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function build_form_submit_comment($location)
{
  $thread_id = isset($_GET["thread_id"]) ? intval($_GET["thread_id"]) : warning("Query is incorrect.", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : "";
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : "";
  $password = isset($_POST["password"]) ? $_POST["password"] : "";
  
  $name = $name === "" && isset($_COOKIE[BBS_NAME."name"]) ? $_COOKIE[BBS_NAME."name"] : $name;
  
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=submit_comment&amp;thread_id="
    .$thread_id."\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Name".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"name\" value=\""
    .$name."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Comment".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><textarea name=\"comment\" rows=\"10\" cols=\"40\" class=\"comment\">"
    .$comment."</textarea><br />You can write source code in &lt;pre&gt;~&lt;/pre&gt;.</td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Your Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\""
    ." value=\"".$password."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $captcha_answer = rand(1000, 9999);
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Authentication".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"captcha\" value=\"\" class=\"captcha\" />\n";
  $str .= "<input type=\"hidden\" name=\"captcha_answer\" value=\"".$captcha_answer."\" />\n";
  $str .= " &lt;-- Please input '".$captcha_answer."'</td>\n";
  $str .= "</tr>\n";
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      $str .= "<tr>\n";
      $str .= "<td class=\"form_label\">File".($i + 1)."</td>\n";
      $str .= "<td class=\"form_field\"><input type=\"file\" name=\"file".$i."\" class=\"file\" /></td>\n";
      $str .= "</tr>\n";
    }
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\" name=\"submit\""
    ." value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function build_form_edit_comment($location)
{
  $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
  
  $db = connect_db();
  
  $sql = "select name, comment from comment where comment_id=".$comment_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Assertion error", __LINE__);
  $name = isset($_POST["name"]) ? $_POST["name"] : $row["name"];
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : $row["comment"];
  
  $sql = "select * from file where comment_id=".$comment_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  while($row = @mysqli_fetch_assoc($result))
    {
      $save_name["file".$row["position"]] = $row["save_name"];
      $submit_name["file".$row["position"]] = $row["submit_name"];
    }
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=edit_comment&amp;comment_id="
    .$comment_id."\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Name".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"text\" name=\"name\" value=\""
    .$name."\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Comment".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><textarea name=\"comment\" rows=\"10\" cols=\"40\" class=\"comment\">"
    .$comment."</textarea><br />You can write source code in &lt;pre&gt;~&lt;/pre&gt;.</td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Your Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\""
    ." value=\"\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      $str .= "<tr>\n";
      $str .= "<td class=\"form_label\">File".($i + 1)."</td>\n";
      $str .= "<td class=\"form_field\">\n";
      $str .= "<input type=\"file\" name=\"file".$i."\" class=\"file\" /> \n";
      if(isset($save_name["file".$i]))
        {
          $str .= "<input type=\"checkbox\" name=\"delfile".$i
            ."\" value=\"1\" />delete<a href=\"".BBS_FILE_DIR
            .$save_name["file".$i]."\">".convert_htmlstr($submit_name["file".$i])."</a>\n";
        }
      $str .= "</td>\n";
      $str .= "</tr>\n";
    }
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\" name=\"submit\""
    ." value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function build_form_delete_comment($location)
{
  $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
    
  $str = "";
  
  $str .= "<form method=\"post\" action=\"".$_SERVER["PHP_SELF"]
    ."?tag=".$location["tag"]."&amp;action=delete_comment&amp;comment_id="
    .$comment_id."\" enctype=\"multipart/form-data\">\n";
  $str .= "<div class=\"table\">\n";
  $str .= "<table class=\"form\" summary=\"form\">\n";
  
  $str .= "<tr>\n";
  $str .= "<td class=\"form_label\">Your Password".MARK4."</td>\n";
  $str .= "<td class=\"form_field\"><input type=\"password\" name=\"password\""
    ." value=\"\" class=\"item\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "<tr>\n";
  $str .= "<td colspan=\"2\" class=\"form_submit\"><input type=\"submit\" name=\"submit\""
    ." value=\"Submit\" class=\"submit\" /></td>\n";
  $str .= "</tr>\n";
  
  $str .= "</table>\n";
  $str .= "</div>\n";
  $str .= "</form>\n";
  
  return $str;
}

function check_admin_password()
{
  $password = isset($_POST["password"]) ? $_POST["password"] : "";
  
  if($password !== BBS_ADMIN_PASSWORD)
    {
      return "Admin password is wrong!";
    }
  return "";
}

function check_comment_password()
{
  $comment_id = isset($_GET["comment_id"]) ? intval($_GET["comment_id"]) : warning("Query is incorrect.", __LINE__);
  $password = isset($_POST["password"]) ? $_POST["password"] : "";
  
  $db = connect_db();
  
  $sql = "select password from comment where comment_id=".$comment_id;
  $result = @mysqli_query($db, $sql) or error("Database error", __LINE__);
  $row = @mysqli_fetch_assoc($result);
  if($row == NULL) error("Assertion error", __LINE__);
  
  @mysqli_close($db) or error("Database error", __LINE__);
  
  if($password !== $row["password"] && $password !== BBS_ADMIN_PASSWORD)
    {
      return "Password is wrong!";
    }
  return "";
}

function check_captcha()
{
  $captcha = isset($_POST["captcha"]) ? intval($_POST["captcha"]) : "";
  $captcha_answer = isset($_POST["captcha_answer"]) ? intval($_POST["captcha_answer"]) : "";
  
  if($captcha != $captcha_answer)
    {
      return "Key number is wrong!";
    }
  return "";
}

function check_thread()
{
  $title = isset($_POST["title"]) ? $_POST["title"] : "";
  $name = isset($_POST["name"]) ? $_POST["name"] : "";
  
  if(strlen($title) <= 0)
    {
      return "Title is empty!";
    }
  if(100 < strlen($title))
    {
      return "Title is too long!";
    }
  if(strlen($name) <= 0)
    {
      return "Name is empty!";
    }
  if(100 < strlen($name))
    {
      return "Name is too long!";
    }
  return "";
}

function check_comment()
{
  $name = isset($_POST["name"]) ? $_POST["name"] : "";
  $comment = isset($_POST["comment"]) ? $_POST["comment"] : "";
  $password = isset($_POST["password"]) ? $_POST["password"] : "";
  
  if(strlen($name) <= 0)
    {
      return "Name is empty!";
    }
  if(100 < strlen($name))
    {
      return "Name is too long!";
    }
  if(strlen($comment) <= 0)
    {
      return "Comment is empty!";
    }
  if(1000000 < strlen($comment))
    {
      return "Comment is too long!";
    }
  if(strlen($password) <= 0)
    {
      return "Password is empty!";
    }
  if(100 < strlen($password))
    {
      return "Password is too long!";
    }
  
  for($i = 0; $i < BBS_FILE_NUM_MAX; $i++)
    {
      if(isset($_FILES["file".$i]["tmp_name"])
         && is_uploaded_file($_FILES["file".$i]["tmp_name"])
         && filesize($_FILES["file".$i]["tmp_name"]) > BBS_FILE_SIZE_MAX)
        {
          return "File size must be under ".(BBS_FILE_SIZE_MAX / 1024)."KB!";
        }
    }
  return "";
}

function connect_db()
{
  $db = @mysqli_connect(BBS_DB_HOST, BBS_DB_USER, BBS_DB_PASSWORD) or error("Database error", __LINE__);
  @mysqli_select_db($db, BBS_DB_NAME) or error("Database error", __LINE__);
  return $db;
}

function set_cookie()
{
  $period = time() + 1000 * 24 * 60 * 60;
  if(isset($_POST["name"]))
    {
      setcookie(BBS_NAME."name", $_POST["name"], $period);
    }
  setcookie(BBS_NAME."time", time(), $period);
  return;
}

function convert_datestr($time)
{
  $str = date("Y/m/d H:i:s", $time);
  $str = preg_replace("/^([\d\/]+)\s+([\d:]+)$/", "\\1<br />\\2", $str);
  return $str;
}

function convert_dbstr($db, $str)
{
  if(get_magic_quotes_gpc())
    {
      $str = stripslashes($str);
    }
  $str = @mysqli_real_escape_string($db, $str);
  return $str;
}

function convert_htmlstr($str)
{
  $str = htmlspecialchars($str);
  $str = nl2br($str);
  return $str;
}
*/

?>
