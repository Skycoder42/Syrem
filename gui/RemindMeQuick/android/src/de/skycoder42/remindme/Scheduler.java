package de.skycoder42.remindme;

import android.content.Context;

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
}
