<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>EventExpressionParser</name>
    <message>
        <location filename="eventexpressionparser.cpp" line="+444"/>
        <source>Unknown Error</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unable to parse expression. Could not understand beginning of the expression.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Unable to parse expression. Was able to parse until position %L1, but could not understand the part after this position.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Detected duplicate expression scope. Subterm &quot;%1&quot; conflicts with a previous subterm.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Detected more then one loop expression. Subterm &quot;%1&quot; conflicts with a previous loop subterm.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Detected more then one timespan expression. Subterm &quot;%1&quot; conflicts with a previous timespan subterm.&lt;br/&gt;&lt;b&gt;Note:&lt;/b&gt; You can specify multile timespans in one expression using something like &quot;in 2 hours and 20 minutes&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Detected more then one &quot;from&quot; limiter expression. Subterm &quot;%1&quot; conflicts with a previous &quot;from&quot; limiter subterm.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Detected more then one &quot;until&quot; limiter expression. Subterm &quot;%1&quot; conflicts with a previous &quot;until&quot; limiter subterm.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Detected unexpected limiter expression. Subterm &quot;%1&quot; can only be used if a loop expression was previously used.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Found an absolute subterm that is not the greatest scope. Detected after checking the subterm until %L1 for inconsistencies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Found a timespan expression of a logically smaller scope than a timepoint expression. This is not supported. Detected after checking the subterm until %L1 for inconsistencies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Found a loop expression that is used as loop limiter. Limiters cannot be loops. Detected after checking the subterm until %L1 for inconsistencies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Found a limiter expression of a logically smaller scope than the fencing part of a loop expression. This is currently not supported due to a too high complexity. Detected after checking the subterm until %L1 for inconsistencies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Tried to use a looped expression to get a single date. You must use a normal, non-loop expression instead.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>The given expression, when applied to the current date, would evalute to the past. Expressions must always evalute to a timepoint in the future.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>From and until limiter of the given expression are valid, but from points to a timepoint further in the future than until. The until expression must always be further in the past than the from expression.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>The given looped expression, when applied to the current date to get the first occurence date, did not return a valid date. This typically indicates that the application did not find any dates from the current date on that match the expression. Make shure your expression evalutes to at least one date in the future.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+163"/>
        <source>within {%1} </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>every {%1}</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source> from {%1}</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source> until {%1}</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="reminder.cpp" line="+153"/>
        <source>The snooze time must be in the future of the normal reminder time and not in the past of it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="terms.cpp" line="+988"/>
        <source>at </source>
        <comment>TimePrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source> o&apos;clock</source>
        <comment>TimeSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>hh:mm ap|h:mm ap|hh:m ap|h:m ap|hh ap|h ap|hh:mm AP|h:mm AP|hh:m AP|h:m AP|hh AP|h AP|hh:mm|h:mm|hh:m|h:m|hh|h</source>
        <comment>TimePattern</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source>on |on the |the </source>
        <comment>DatePrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>every |any |all</source>
        <comment>DateLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>dd.MM.yyyy|d.MM.yyyy|dd.M.yyyy|d.M.yyyy|dd. MM. yyyy|d. MM. yyyy|dd. M. yyyy|d. M. yyyy|dd-MM-yyyy|d-MM-yyyy|dd-M-yyyy|d-M-yyyy|dd.MM.yy|d.MM.yy|dd.M.yy|d.M.yy|dd. MM. yy|d. MM. yy|dd. M. yy|d. M. yy|dd-MM-yy|d-MM-yy|dd-M-yy|d-M-yy|dd.MM.|d.MM.|dd.M.|d.M.|dd. MM.|d. MM.|dd. M.|d. M.|dd-MM|d-MM|dd-M|d-M</source>
        <comment>DatePattern</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+13"/>
        <source>%2 past %1:+|%2-past %1:+|%2 to %1:-</source>
        <comment>InvTimeExprPattern</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>hh ap|h ap|hh AP|h AP|hh|h</source>
        <comment>InvTimeHourPattern</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>mm|m</source>
        <comment>InvTimeMinutePattern</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>quarter:15|half:30</source>
        <comment>InvTimeKeywords</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>on |on the |the |next |on next |on the next</source>
        <comment>MonthDayPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source> of</source>
        <comment>MonthDaySuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>every |any |all</source>
        <comment>MonthDayLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>_.|_th|_st|_nd|_rd</source>
        <comment>MonthDayIndicator</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>on |next |on next |on the next</source>
        <comment>WeekDayPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>every |any |all</source>
        <comment>WeekDayLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>in |on |next |on next |on the next |in next |in the next</source>
        <comment>MonthPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>every |any |all</source>
        <comment>MonthLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>in </source>
        <comment>YearPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>in </source>
        <comment>SpanPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>every |all</source>
        <comment>SpanLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source> and</source>
        <comment>SpanConjuction</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>min|mins|minute|minutes</source>
        <comment>SpanKeyMinute</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>hour|hours</source>
        <comment>SpanKeyHour</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>day|days</source>
        <comment>SpanKeyDay</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>week|weeks</source>
        <comment>SpanKeyWeek</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>mon|mons|month|months</source>
        <comment>SpanKeyMonth</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>year|years</source>
        <comment>SpanKeyYear</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>today:0|tomorrow:1</source>
        <comment>KeywordDayspan</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>from</source>
        <comment>LimiterFromPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>until|to</source>
        <comment>LimiterUntilPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>;</source>
        <comment>ExpressionSeperator</comment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::DateTerm</name>
    <message>
        <location line="-944"/>
        <source>yyyy-MM-dd</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>MM-dd</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::InvertedTimeTerm</name>
    <message>
        <location line="+119"/>
        <source>hh:mm</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::KeywordTerm</name>
    <message numerus="yes">
        <location line="+625"/>
        <source>in %n day(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
</context>
<context>
    <name>Expressions::MonthDayTerm</name>
    <message>
        <location line="-516"/>
        <source>%1.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::SequenceTerm</name>
    <message numerus="yes">
        <location line="+429"/>
        <source>%n minute(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n hour(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n day(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n week(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n year(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message>
        <location line="+8"/>
        <source>in %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::TimeTerm</name>
    <message>
        <location line="-800"/>
        <source>hh:mm</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Syrem</name>
    <message>
        <location filename="libsyrem.cpp" line="+52"/>
        <source>&lt;p&gt;&lt;u&gt;Syntax Specification:&lt;/u&gt;&lt;/p&gt;&lt;p&gt;You can enter an &lt;i&gt;&amp;lt;expression&amp;gt;&lt;/i&gt; to define timepoints to remind you. An expression can be: &lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt; &lt;tr&gt;  &lt;td&gt;&lt;b&gt;conjuction &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;&amp;lt;expression&amp;gt; ; &amp;lt;expression&amp;gt; [; &amp;lt;expression&amp;gt; …]&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;timespan &lt;/span&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;in &amp;lt;sequence&amp;gt; [on|at|in &amp;lt;datum&amp;gt;] [&amp;lt;time&amp;gt;]&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;loop &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;every &amp;lt;type&amp;gt; [on|at|in &amp;lt;datum&amp;gt;] [&amp;lt;time&amp;gt;] [from [&amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]] [until [&amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]]&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;point &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;[[on|next] &amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;u&gt;Basic Types:&lt;/u&gt;&lt;br/&gt;The specifications above make use of a bunch of basic types. These types are: &lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt; &lt;tr&gt;  &lt;td&gt;&lt;b&gt;datum &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;&amp;lt;weekday&amp;gt; | &amp;lt;day&amp;gt; | &amp;lt;month&amp;gt; | &amp;lt;mday&amp;gt;&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;sequence &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{int} &amp;lt;span&amp;gt; [and {int} &amp;lt;span&amp;gt; …]&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;type &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;&amp;lt;datum&amp;gt; | &amp;lt;sequence&amp;gt;&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;tpoint &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;&amp;lt;date&amp;gt; | &amp;lt;datum&amp;gt; | &amp;lt;year&amp;gt; | &amp;lt;ahead&amp;gt;&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt; &lt;tr&gt;  &lt;td&gt;&lt;b&gt;time &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;[at] {hh[:mm]} | {hh] oclock&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;date &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{dd-MM-yyyy}&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;weekday &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{Monday..Sunday}&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;day &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{01..31}.&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;month &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{01..12} | {Janurary..December}&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;year &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;yyyy&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;mday &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;{dd-MM}&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;span &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;minute | hour | day | week | month | year&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;tr&gt;  &lt;td&gt;&lt;b&gt;ahead &lt;/b&gt;&lt;/td&gt;  &lt;td&gt;:= &lt;i&gt;tomorrow | today&lt;/i&gt;&lt;/td&gt; &lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;u&gt;Custom Formats:&lt;/u&gt;&lt;br/&gt;For many of the formats above, you can customize the keywords to be used in the settings. For the &amp;lt;date&amp;gt; and &amp;lt;mday&amp;gt; formats, you can even define custom formats to be accepted. Check the settings for details on those formats.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>settings.xml</name>
    <message>
        <location filename="../../build-Syrem-Desktop_Qt_5_11_1_GCC_64bit-Debug/lib/syncedsettings.cpp" line="+29"/>
        <source>09:00</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>in 20 minutes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>in 1 hour</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>in 3 hours</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>tomorrow</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>in 1 week on Monday</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
