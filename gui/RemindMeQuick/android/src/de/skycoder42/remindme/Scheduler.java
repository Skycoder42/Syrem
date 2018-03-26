package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;

import android.app.PendingIntent;
import android.app.AlarmManager;

import android.support.v4.app.AlarmManagerCompat;

public class Scheduler {
	private Context context;

	public Scheduler(Context context) {
		this.context = context;
	}

	public void createSchedule(String remId, int versionCode, boolean important, long triggerAt) {
		PendingIntent pending = Globals.createPending(context, Globals.Actions.ActionScheduler, remId, versionCode);
		AlarmManager manager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		if(important)
			AlarmManagerCompat.setExactAndAllowWhileIdle(manager, AlarmManager.RTC_WAKEUP, triggerAt, pending);
		else
			manager.setWindow(AlarmManager.RTC_WAKEUP, triggerAt, 5 * 60 * 1000, pending);//max 5 min window
	}

	public void cancelSchedule(String remId) {
		PendingIntent pending = Globals.createPending(context, Globals.Actions.ActionScheduler, remId, 0);
		AlarmManager manager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		manager.cancel(pending);
	}

	public static void scheduleAutoCheck(Context context) {
		Globals.Actions action = Globals.Actions.ActionRefresh;

		Intent intent = new Intent(action.getAction(), null, context, RemindmeService.class);
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);

		PendingIntent pending = null;
		if (Globals.isOreo())
			pending = PendingIntent.getForegroundService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			pending = PendingIntent.getService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);

		AlarmManager alarm = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);
		alarm.setRepeating(AlarmManager.RTC_WAKEUP,
			System.currentTimeMillis() + AlarmManager.INTERVAL_HOUR,
			AlarmManager.INTERVAL_HOUR,
			pending);
	}
}
