# Remind-Me Reminder Syntax

## Base types:
```
time     := [at] {hh[:mm]}
date     := {dd-MM-yyyy}
weekday  := {Monday..Sunday}
day      := {01..31}.
month    := {01..12} | {Janurary..December}
year     := (any valid year)
mday     := {dd-MM}
span     := minute | hour | day | week | month | year
ahead    := tomorrow | today

datum  := <weekday> | <day> | <month> | <mday>
type   := <datum> | <span>
tpoint := <date> | <datum> | <year> | <ahead>
```

## Expressions:
```
expression := <conjunction> | <offset> | <timespan> | <loop> | <point>

conjuction := <expression> and|; <expression> [and|; <expression> ...]
timespan	   := in {int} <span> [on|at|in <datum>] [<time>]
loop       := every <type> [on|at|in <datum>] [<time>] [from <tpoint> [<time>]] [until <tpoint> [<time>]]
point	   := [on|next] <tpoint> [<time>]
```