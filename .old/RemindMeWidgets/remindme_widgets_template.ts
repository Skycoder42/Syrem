<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>CreateReminderDialog</name>
    <message>
        <location filename="createreminderdialog.ui" line="+14"/>
        <source>Create Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>&amp;Text:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Enter a description of what to be reminded</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;When:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Expression to describe when the reminder should be triggered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:12pt; text-decoration: underline;&quot;&gt;Syntax Specification:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;You can enter an &lt;span style=&quot; font-style:italic;&quot;&gt;&amp;lt;expression&amp;gt;&lt;/span&gt; to define timepoints to remind you. An expression can be: &lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;conjuction &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;&amp;lt;expression&amp;gt; ; &amp;lt;expression&amp;gt; [; &amp;lt;expression&amp;gt; …]&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;timespan &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;in &amp;lt;sequence&amp;gt; [on|at|in &amp;lt;datum&amp;gt;] [&amp;lt;time&amp;gt;]&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;loop &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;every &amp;lt;type&amp;gt; [on|at|in &amp;lt;datum&amp;gt;] [&amp;lt;time&amp;gt;] [from [&amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]] [until [&amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]]&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;point &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;[[on|next] &amp;lt;tpoint&amp;gt;] [&amp;lt;time&amp;gt;]&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;Basic Types:&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&lt;br/&gt;&lt;/span&gt;The specifications above make use of a bunch of basic types. These types are: &lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;datum &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;&amp;lt;weekday&amp;gt; | &amp;lt;day&amp;gt; | &amp;lt;month&amp;gt; | &amp;lt;mday&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;sequence &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{int} &amp;lt;span&amp;gt; [and {int} &amp;lt;span&amp;gt; …]&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;type &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;&amp;lt;datum&amp;gt; | &amp;lt;sequence&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;tpoint &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;&amp;lt;date&amp;gt; | &amp;lt;datum&amp;gt; | &amp;lt;year&amp;gt; | &amp;lt;ahead&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;table border=&quot;0&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; cellspacing=&quot;2&quot; cellpadding=&quot;0&quot;&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;time &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;[at] {hh[:mm]} | {hh] oclock&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;date &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{dd-MM-yyyy}&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;weekday &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{Monday..Sunday}&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;day &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{01..31}.&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;month &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{01..12} | {Janurary..December}&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;year &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;yyyy&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;mday &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;{dd-MM}&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;span &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;minute | hour | day | week | month | year&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;ahead &lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;:= &lt;span style=&quot; font-style:italic;&quot;&gt;tomorrow | today&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;Custom Formats:&lt;br/&gt;&lt;/span&gt;For many of the formats above, you can customize the keywords to be used in the settings. For the &amp;lt;date&amp;gt; and &amp;lt;mday&amp;gt; formats, you can even define custom formats to be accepted. Check the settings for details on those formats.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;Important:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Important reminders will show up with additional visual and audio signals to be seens easier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Expression &amp;Syntax</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Show the expression syntax</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>KdeNotifier</name>
    <message>
        <location filename="kdenotifier.cpp" line="+58"/>
        <source>%1 — Important Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 — Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Open GUI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Complete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Snooze</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+37"/>
        <source>%1 — Error</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="mainwindow.ui" line="+14"/>
        <source>Manager Reminders</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+66"/>
        <source>&amp;File</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Edit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
        <source>&amp;Help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+28"/>
        <source>&amp;Add Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Create a new reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>&amp;Delete Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Deletes the selected reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>&amp;Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>&amp;Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>&amp;About</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+9"/>
        <source>&amp;Quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+12"/>
        <source>&amp;Complete Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Completes and deletes the selected reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>&amp;Snooze Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Snooze the selected reminder until a later timepoint</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="+77"/>
        <source>A simple reminder application for desktop and mobile, with synchronized reminder.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>BSD 3 Clause</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ReminderProxyModel</name>
    <message>
        <location line="+62"/>
        <source>Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Due on</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+21"/>
        <source>&lt;br/&gt;&lt;i&gt;This is an important reminder&lt;/i&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+30"/>
        <source>
Reminder will repeatedly trigger, not only once</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
Reminder has been snoozed until the displayed time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
Reminder has been triggered and needs a reaction!</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SnoozeDialog</name>
    <message>
        <location filename="snoozedialog.cpp" line="+11"/>
        <source>Snooze Reminder</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SnoozeTimesEdit</name>
    <message>
        <location filename="snoozetimesedit.ui" line="+14"/>
        <source>Form</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+61"/>
        <source>&amp;Add Time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>&amp;Remove Time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Move &amp;Up</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Move &amp;Down</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>WidgetsNotifier</name>
    <message>
        <location filename="widgetsnotifier.cpp" line="+51"/>
        <source>%1 — Important Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 — Reminder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 — Error</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+21"/>
        <source>An error occured!</source>
        <translation type="unfinished"></translation>
    </message>
    <message numerus="yes">
        <location line="+74"/>
        <source>%1 — %n active reminder(s)</source>
        <translation type="unfinished">
            <numerusform></numerusform>
        </translation>
    </message>
    <message>
        <location line="+4"/>
        <source>%1 — An error occured!</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>WidgetsSnoozeDialog</name>
    <message>
        <location filename="widgetssnoozedialog.cpp" line="+78"/>
        <source>Invalid Snooze</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Triggered Reminders</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Please select an action for each of your triggered reminders:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+33"/>
        <source>&amp;Complete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>&amp;Default Snooze</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+10"/>
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
    <message>
        <location line="+4"/>
        <source>&amp;Snooze</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
