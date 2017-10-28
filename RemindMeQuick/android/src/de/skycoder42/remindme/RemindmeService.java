package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;
import android.app.PendingIntent;
import android.app.AlarmManager;

import org.qtproject.qt5.android.bindings.QtService;

public class RemindmeService extends QtService {
	public class LocalBinder extends Binder {
		//empty class
	}

	private final IBinder _binder = new LocalBinder();

	public static void startService(Context context) {
		context.startService(new Intent(context, RemindmeService.class));
	}

	@Override
	public IBinder onBind(Intent intent) {
		return _binder;
	}

	public void createSchedule(int id, boolean important, long triggerAt)
	{
		PendingIntent pending = createPending(id);
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		if(important)
			manager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, triggerAt, pending);
		else
			manager.setWindow(AlarmManager.RTC_WAKEUP, triggerAt, 5 * 60 * 1000, pending);//max 5 min window
	}

	public void cancleSchedule(int id)
	{
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		manager.cancel(createPending(id));
	}

	private PendingIntent createPending(int id)
	{
		Intent intent = new Intent(this, AlarmReceiver.class);
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
		return PendingIntent.getBroadcast(this, id, intent, PendingIntent.FLAG_UPDATE_CURRENT);
	}
}
