<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>EventExpressionParser</name>
    <message>
        <location filename="eventexpressionparser.cpp" line="+451"/>
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
        <location filename="terms.cpp" line="+1172"/>
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
        <location line="+3"/>
        <source>###empty###</source>
        <comment>DateSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>###empty###</source>
        <comment>DateLoopSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
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
        <source>on |on the |the |next |on next |on the next </source>
        <oldsource>on |on the |the |next |on next |on the next</oldsource>
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
        <location line="+6"/>
        <source>###empty###</source>
        <comment>MonthDayLoopSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>_.|_th|_st|_nd|_rd</source>
        <comment>MonthDayIndicator</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>on |next |on next |on the next </source>
        <oldsource>on |next |on next |on the next</oldsource>
        <comment>WeekDayPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>###empty###</source>
        <comment>WeekDaySuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>###empty###</source>
        <comment>WeekDayLoopSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>in |on |next |on next |on the next |in next |in the next </source>
        <oldsource>in |on |next |on next |on the next |in next |in the next</oldsource>
        <comment>MonthPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>###empty###</source>
        <comment>MonthSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>###empty###</source>
        <comment>MonthLoopSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>###empty###</source>
        <comment>YearSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>###empty###</source>
        <comment>SpanSuffix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="-79"/>
        <source>every |any |all |on every |on any |on all </source>
        <comment>DateLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+37"/>
        <source>every |any |all |on every |on any |on all </source>
        <comment>MonthDayLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>every |any |all |on every |on any |on all </source>
        <comment>WeekDayLoopPrefix</comment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>every |any |all |on every |on any |on all </source>
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
        <source>every |all </source>
        <oldsource>every |all</oldsource>
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
        <location line="-1108"/>
        <source>yyyy-MM-dd</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>MM-dd</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {1..31}.{1..12}.[&lt;year&gt;] %2 (and other similar date-formats)</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::InvertedTimeTerm</name>
    <message>
        <location line="+126"/>
        <source>hh:mm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+11"/>
        <source>time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {half|quarter|0..60} past|to {0..24} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::KeywordTerm</name>
    <message numerus="yes">
        <location line="+742"/>
        <source>in %n day(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message>
        <location line="+9"/>
        <source>keyword</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>{today|tomorrow}</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::MonthDayTerm</name>
    <message>
        <location line="-635"/>
        <source>%1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>day</source>
        <oldsource>date</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {1..31}{.|th|st|nd|rd} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::MonthTerm</name>
    <message>
        <location line="+263"/>
        <source>month</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {Jan[uary]..Dec[ember]} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::SequenceTerm</name>
    <message numerus="yes">
        <location line="+238"/>
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
    <message>
        <location line="+12"/>
        <source>span</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 [&lt;number&gt;] {min[utes]|hours|days|weeks|months|years} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::TimeTerm</name>
    <message>
        <location line="-965"/>
        <source>hh:mm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+11"/>
        <source>time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {0..24}[:{0..60}] %2 (and other similar time-formats)</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::WeekDayTerm</name>
    <message>
        <location line="+554"/>
        <source>weekday</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {Mon[day]..Sun[day]} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Expressions::YearTerm</name>
    <message>
        <location line="+190"/>
        <source>year</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 {&lt;4-digit-number&gt;} %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Syrem</name>
    <message>
        <location filename="libsyrem.cpp" line="+91"/>
        <source>&lt;h2&gt;Reminder creation help&lt;/h2&gt;&lt;p&gt;	Use the &quot;when&quot; field to enter an expression describen when you want to be reminded.	The app is able to understand common expressions like &quot;in 3 hours&quot; or &quot;in April on the 24th&quot;.	The specification below can be used to get an overview of all the possible terms you can enter.&lt;/p&gt;&lt;p&gt;	&lt;span style=&quot;text-decoration: underline;&quot;&gt;Pro Tip:&lt;/span&gt; 	If you&apos;re unsure if what you enter is correct and correctly understood, 	you can enable syntax verification in the settings to let the app tell you how it understood what you entered.&lt;/p&gt;&lt;p&gt;	&lt;h3&gt;Syntax Specification&lt;/h3&gt;&lt;/p&gt;&lt;p&gt;	Expressions are typically made up of multiple subterms, e.g. &quot;in April on the 24th&quot; consits of the subterm &quot;in April&quot; and &quot;on the 24th&quot;.	Generally speaking, you can combine any of the supported subterms in any order to create expressions, as long as they are still logical.	For example, &quot;at 15:00 in 3 hours&quot; is not possible, as the two subterms conflict each other.&lt;/p&gt;&lt;p&gt;	&lt;h4&gt;Possible Subterms&lt;/h4&gt;&lt;/p&gt;&lt;p&gt;	The following syntaxes are the different subterms available.	The first table shows all singular expressions, i.e. expression that when evaluted result in a single occurence.&lt;table&gt;&lt;tbody&gt;	%1&lt;/tbody&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;	The next table shows expressions for repeated events.	Please note that all repeated expression are by default &quot;infinitely&quot;, i.e. they repeat until all eternity or until they get manually deleted by you.	However, you can use so called limiter expression to specifiy a range for the to occur.	The syntax is:&amp;nbsp;&lt;em&gt;{loop-term} [from {limiter-term}] [until|to {limiter-term}]&lt;/em&gt;.	Limiter-terms are just like any normal expression you would enter, but the can&apos;t be looped.	You can specify any limiter in any order. The loop-term is one of the terms from below:&lt;table&gt;&lt;tbody&gt;	%2&lt;/tbody&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;	&lt;h4&gt;Logical restrains&lt;/h4&gt;&lt;/p&gt;&lt;p&gt;	While generally speaking, you can combine those terms in any order, as long as they don&apos;t conflict each other, there are a few further restrains, 	some of logical and some of technical origin. They are listed below:&lt;ul&gt;	&lt;li&gt;There can only be a single loop term per expression&lt;/li&gt;	&lt;li&gt;Limiters must be unique and &quot;until&quot; in the future of &quot;from&quot;&lt;/li&gt;	&lt;li&gt;There can only be a single timespan per expression&lt;/li&gt;	&lt;li&gt;That timespan must have the greatest scope&lt;/li&gt;	&lt;li&gt;Limiters cannot be smaller than the scope of the loop expressions fence&lt;/li&gt;	&lt;li&gt;All expression must evaluate to the future&lt;/li&gt;	&lt;li&gt;Loops must have at least one valid occurence&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;&lt;p&gt;	&lt;h3&gt;Examples&lt;/h3&gt;&lt;/p&gt;&lt;p&gt;	&lt;em&gt;&amp;lt;Coming soon...&amp;gt;&lt;/em&gt;&lt;/p&gt;</source>
        <oldsource>&lt;p&gt;	Use the &quot;when&quot; field to enter an expression describen when you want to be reminded.	The app is able to understand common expressions like &quot;in 3 hours&quot; or &quot;in April on the 24th&quot;.	The specification below can be used to get an overview of all the possible terms you can enter.&lt;/p&gt;&lt;p&gt;	&lt;span style=&quot;text-decoration: underline;&quot;&gt;Pro Tip:&lt;/span&gt; 	If you&apos;re unsure if what you enter is correct and correctly understood, 	you can enable syntax verification in the settings to let the app tell you how it understood what you entered.&lt;/p&gt;&lt;p&gt;	&lt;strong&gt;&lt;span style=&quot;text-decoration: underline;&quot;&gt;Syntax Specification&lt;/span&gt;&lt;/strong&gt;&lt;/p&gt;&lt;p&gt;	Expressions are typically made up of multiple subterms, e.g. &quot;in April on the 24th&quot; consits of the subterm &quot;in April&quot; and &quot;on the 24th&quot;.	Generally speaking, you can combine any of the supported subterms in any order to create expressions, as long as they are still logical.	For example, &quot;at 15:00 in 3 hours&quot; is not possible, as the two subterms conflict each other.&lt;/p&gt;&lt;p&gt;	&lt;strong&gt;Possible Subterms&lt;/strong&gt;&lt;/p&gt;&lt;p&gt;	The following syntaxes are the different subterms available.	The first table shows all singular expressions, i.e. expression that when evaluted result in a single occurence.&lt;table&gt;&lt;tbody&gt;	%1&lt;/tbody&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;	The next table shows expressions for repeated events.	Please note that all repeated expression are by default &quot;infinitely&quot;, i.e. they repeat until all eternity or until they get manually deleted by you.	However, you can use so called limiter expression to specifiy a range for the to occur.	The syntax is:&amp;nbsp;&lt;em&gt;{loop-term} [from {limiter-term}] [until|to {limiter-term}]&lt;/em&gt;.	Limiter-terms are just like any normal expression you would enter, but the can&apos;t be looped.	You can specify any limiter in any order. The loop-term is one of the terms from below:&lt;table&gt;&lt;tbody&gt;	%2&lt;/tbody&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;	&lt;strong&gt;Logical restrains&lt;/strong&gt;&lt;/p&gt;&lt;p&gt;	While generally speaking, you can combine those terms in any order, as long as they don&apos;t conflict each other, there are a few further restrains, 	some of logical and some of technical origin. They are listed below:&lt;ul&gt;	&lt;li&gt;There can only be a single loop term per expression&lt;/li&gt;	&lt;li&gt;Limiters must be unique and &quot;until&quot; in the future of &quot;from&quot;&lt;/li&gt;	&lt;li&gt;There can only be a single timespan per expression&lt;/li&gt;	&lt;li&gt;That timespan must have the greatest scope&lt;/li&gt;	&lt;li&gt;Limiters cannot be smaller than the scope of the loop expressions fence&lt;/li&gt;	&lt;li&gt;All expression must evaluate to the future&lt;/li&gt;	&lt;li&gt;Loops must have at least one valid occurence&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;&lt;p&gt;	&lt;span style=&quot;text-decoration: underline;&quot;&gt;&lt;strong&gt;Examples&lt;/strong&gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;	&lt;em&gt;&amp;lt;Coming soon...&amp;gt;&lt;/em&gt;&lt;/p&gt;</oldsource>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>settings.xml</name>
    <message>
        <location filename="../../build-Syrem-Desktop_Qt_5_11_1_GCC_64bit-Release/lib/syncedsettings.cpp" line="+30"/>
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
