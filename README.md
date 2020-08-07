# BackupSystem
Only for Databases Backup with Cron Support

Before Add to Cron Jobs (Only for C)
``` bash
yum install json-c-devel
yum install MariaDB-shared.x86_64

Build++
gcc service.c -std=c11 `mysql_config --cflags --libs` -ljson-c -o service
```

Cron Command (Once an hour for C)
``` bash
0 * * * * /APIS/backupsystem/service >> /CustomLogs/backupsystem.log 2>&1
```

Cron Command (Once an hour for PHP)
``` bash
0 * * * * /opt/alt/php-fpm73/usr/bin/php -f /backupsystem/service.php >> /CustomLogs/backupsystem.log 2>&1
```

```bash
@reboot : Run once after reboot.
@yearly : Run once a year, ie. 0 0 1 1 *
@annually : Run once a year, ie. 0 0 1 1 *
@monthly : Run once a month, ie. 0 0 1 * *
@weekly : Run once a week, ie. 0 0 * * 0
@daily : Run once a day, ie. 0 0 * * *
@hourly : Run once an hour, ie. 0 * * * *
````
