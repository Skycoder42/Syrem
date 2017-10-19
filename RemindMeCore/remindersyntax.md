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

datum    := <weekday> | <day> | <month> | <mday>
sequence := {int} <span> [and {int} <span> ...]
type     := <datum> | <sequence>
tpoint   := <date> | <datum> | <year> | <ahead>
```

## Expressions:
```
expression := <conjunction> | <offset> | <timespan> | <loop> | <point>

conjuction := <expression> ; <expression> [; <expression> ...]
timespan   := in <sequence> [on|at|in <datum>] [<time>]
loop       := every <type> [on|at|in <datum>] [<time>] [from <tpoint> [<time>]] [until <tpoint> [<time>]]
point	   := [on|next] <tpoint> [<time>]
```

## Regular Expressions:
```
time: ^(?:at )?(\d{1,2}:\d{2}|\d{1,2} oclock)$
span: ^(\d+) (\w+)$				[with int]

datum: ^(?:(<weekday>)|(\d+)\.|(\d+)|(<month>)|(.+?))$
sequence: ^((?:\d+) (?:\w+)(?: and (?:\d+) (?:\w+))*)$
type: ^(?:<sequence>|(.+?))$
tpoint: ^(?:(today)|(tomorrow)|(\d{4})|(.*?))$

conjuction: (\s*;\s*) 				[seperator]
timespan: ^in <sequence>(?:(?: on| at| in) (.+?))??(?: <time>)?$
loop: ^every (.+?)(?:(?: on| at| in) (.+?))??(?: <time>)?(?: from ((?:(?!until).)*))?(?: until (.*))?$
point: ^(?:on |next )?(.+?)(?: <time>)?$
```