# C Playground - `monitor-battery`

Check the battery status via the appropriate sysfs path. Notify if the battery
is low (<10%) or suspend the system if critical (<5%).

It is used to emulate the behavior of full featured desktop environments. Make
sure it is called via a system that continuously can call a program (e.g. cron
or systemd-timers).

## License

See this monorepo's [LICENSE](../LICENSE).
