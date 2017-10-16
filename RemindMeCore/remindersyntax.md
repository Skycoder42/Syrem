# Remind-Me Reminder Syntax

## Base types:
```
date    := dd-MM-yyyy (or any other accepted format)
time    := hh[:mm] (or any other accepted format)
weekday := {Monday..Sunday}
day     := {01..31}.
month   := {01..12} | {Janurary..December}
year    := (any valid year)
mday	    := <day><month>

datum  := <weekday> | <day> | <month> | <mday> [[at] <time>]
tpoint := <date> | <datum> | <year> | tomorrow | today [[at] <time>]
type   := <datum> | day | week | month | year [[at] <time>]
```

## Syntax:
```
reminder := <conjunction> | <offset> | <loop> | <point> | <range>

conjuction := <reminder> and|, <reminder> [and|, <reminder> ...]
offset	   := next <type> [on|at|in <datum>]
loop       := every <type> [on|at|in <datum>] [from <tpoint>] [until <tpoint>]
point	   := [on] <tpoint>
range      := <tpoint> - <tpoint>
```