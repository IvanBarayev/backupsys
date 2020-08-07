<?PHP
/*ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);*/
define("PTH", __DIR__);
define("conf", json_decode(file_get_contents(PTH . "/config.json"), TRUE));

$conn = mysqli_connect(conf['dbhost'], conf['dbuser'], conf['dbpass'], '', conf['dbport']);
mysqli_set_charset($conn, "utf8");

$date = date('Y-m-d');
$time = date('H:i:s');

if (!$conn) {
    die('Could not connect: ' . mysqli_error());
}

$sql = "SELECT `SCHEMA_NAME` FROM `information_schema`.`SCHEMATA` WHERE `SCHEMA_NAME` NOT IN ('information_schema','performance_schema','mysql','postfix','oauthv2','roundcube')";

$result = mysqli_query($conn, $sql);
$i = 1;

$MainDir = '/Backups';
if(!is_dir($MainDir)){
    mkdir($MainDir, 0755);
}

$MainSubDir = '/Backups/DB';
if(!is_dir($MainSubDir)){
    mkdir($MainSubDir, 0755);
}

$Dir = '/Backups/DB/'.$date;
if(!is_dir($Dir)){
    mkdir($Dir, 0755);
}

$DirDet = '/Backups/DB/'.$date.'/'.$time;
if(!is_dir($DirDet)){
    mkdir($DirDet, 0755);
}

while($r=mysqli_fetch_array($result,MYSQLI_ASSOC)){
    $filename = $r["SCHEMA_NAME"] . "_" . date('Y-m-d H:i:s').".sql.gz";
    $command = 'mysqldump --extended-insert --events --no-create-db --no-create-info --no-set-names --skip-add-locks --skip-comments --skip-triggers --skip-tz-utc --net_buffer_length=25000 '.$r["SCHEMA_NAME"].' | gzip > '.$DirDet.'/'.$r["SCHEMA_NAME"].'.sql.gz';
    //$command = 'mysqldump --extended-insert -R --triggers --events --net_buffer_length=25000 '.$r["SCHEMA_NAME"].' | gzip > '.$DirDet.'/'.$r["SCHEMA_NAME"].'.sql.gz';
    exec($command, $result2);
}
