package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.os.Binder;
import android.app.PendingIntent;
import android.app.Notification;
import android.app.AlarmManager;
import android.app.NotificationManager;
import android.app.NotificationChannel;
import android.net.Uri;
import android.service.notification.StatusBarNotification;
import android.graphics.Color;
import android.graphics.BitmapFactory;

import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationCompat.BigTextStyle;

import org.qtproject.qt5.android.bindings.QtService;

public class RemindmeService extends QtService {
	private static final String ForegroundChannelId = "foreground_channel";
	private static final String NormalChannelId = "normal_channel";
	private static final String ImportantChannelId = "important_channel";
	private static final String ErrorChannelId = "error_channel";
	private static final int ForegroudId = 40;
	private static final int NotifyId = 42;
	private static final int ErrorNotifyId = 66;
	private static final int OpenIntentId = 10;

	private static final String ActionScheduler = "de.skycoder42.remindme.ActionScheduler";
	private static final String ActionComplete = "de.skycoder42.remindme.ActionComplete";
	private static final String ActionDelay = "de.skycoder42.remindme.ActionDelay";

	private final IBinder _binder = new Binder();

	public static void startService(Context context) {
		context.startService(new Intent(context, RemindmeService.class));
	}

	@Override
	public void onCreate() {
		super.onCreate();
		createChannels();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return _binder;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		int result = super.onStartCommand(intent, flags, startId);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, ForegroundChannelId)
			.setContentTitle("Remind-Me Service")
			.setContentText("Synchronizing Reminders…")
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setLocalOnly(true)
			.setOngoing(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			builder.setPriority(NotificationCompat.PRIORITY_MIN);

		startForeground(ForegroudId, builder.build());

		return result;
	}

	public void completeAction() {
		stopForeground(STOP_FOREGROUND_REMOVE);
		stopSelf();
	}

	public void createSchedule(int id, boolean important, long triggerAt) {
		PendingIntent pending = createPending(id, ActionScheduler, null);
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		if(important)
			manager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, triggerAt, pending);
		else
			manager.setWindow(AlarmManager.RTC_WAKEUP, triggerAt, 5 * 60 * 1000, pending);//max 5 min window
	}

	public void cancelSchedule(int id) {
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		manager.cancel(createPending(id, ActionScheduler, null));
	}

	public String[] activeNotifications() {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		StatusBarNotification[] allNots = manager.getActiveNotifications();
		if(allNots == null)
			return null;

		String[] allKeys = new String[allNots.length];
		for(int i = 0; i < allNots.length; i++)
			allKeys[i] = allNots[i].getTag();
		return allKeys;
	}

	public void notify(String id, boolean important, String text) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, important ? ImportantChannelId : NormalChannelId)
			.setContentTitle(important ? "Important Reminder" : "Reminder")
			.setContentText(text)
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
//			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setOnlyAlertOnce(true)
//			.setAutoCancel(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		//TODO addAction, setContentIntent, setDeleteIntent

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
			builder.setDefaults(NotificationCompat.DEFAULT_ALL)
				.setPriority(important ? NotificationCompat.PRIORITY_MAX : NotificationCompat.PRIORITY_DEFAULT);
		}

		Notification notification = builder.build();
		if(important)
			notification.flags |= NotificationCompat.FLAG_INSISTENT;

		manager.notify(id, NotifyId, notification);
	}

	public void notifyError(String text) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		Intent intent = new Intent(this, RemindmeActivity.class);
		PendingIntent pending = PendingIntent.getActivity(this,
			OpenIntentId,
			intent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, ErrorChannelId)
			.setContentTitle("Reminder Error")
			.setContentText(text)
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setAutoCancel(true)
			.setCategory(NotificationCompat.CATEGORY_ERROR);

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			builder.setDefaults(NotificationCompat.DEFAULT_ALL);

		manager.notify(ErrorNotifyId,  builder.build());
	}

	public void cancelNotify(String id) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		manager.cancel(id, NotifyId);
	}

	private PendingIntent createPending(int id, String action, String extra) 	{
		Uri uri = null;
		if(extra != null) {
			uri = new Uri.Builder()
				.scheme("remindme")
				.path(extra)
				.build();
		}

		Intent intent = new Intent(action, uri, this, RemindmeService.class);
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			return PendingIntent.getService(this, id, intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			return PendingIntent.getForegroundService(this, id, intent, PendingIntent.FLAG_UPDATE_CURRENT);
	}

	private void createChannels() {
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			return;

		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		//create normal channel
		NotificationChannel normal = new NotificationChannel(NormalChannelId,
			"Normal Reminders",
			NotificationManager.IMPORTANCE_DEFAULT);
		// Configure the notification channel.
		normal.setDescription("Notifications of normal reminders");
		normal.enableLights(true);
		normal.setLightColor(Color.GREEN);
		normal.enableVibration(true);
		normal.setShowBadge(true);
		manager.createNotificationChannel(normal);

		//create important channel
		NotificationChannel important = new NotificationChannel(ImportantChannelId,
			"Important Reminders",
			NotificationManager.IMPORTANCE_MIN);
		// Configure the notification channel.
		important.setDescription("Notifications of important reminders");
		important.enableLights(true);
		important.setLightColor(Color.RED);
		important.enableVibration(true);
		important.setShowBadge(true);
		important.setBypassDnd(true);
		manager.createNotificationChannel(important);

		//create error channel
		NotificationChannel error = new NotificationChannel(ErrorChannelId,
			"Error Messages",
			NotificationManager.IMPORTANCE_DEFAULT);
		// Configure the notification channel.
		error.setDescription("Notifications for possible internal errors");
		error.enableLights(true);
		error.setLightColor(Color.RED);
		error.enableVibration(true);
		error.setShowBadge(true);
		manager.createNotificationChannel(error);

		//create foreground channel
		NotificationChannel foreground = new NotificationChannel(ForegroundChannelId,
			"Service Status",
			NotificationManager.IMPORTANCE_DEFAULT);
		// Configure the notification channel.
		foreground.setDescription("Status notifications that Remind-Me is synchronizing reminders");
		foreground.enableLights(false);
		foreground.enableVibration(false);
		foreground.setShowBadge(false);
		manager.createNotificationChannel(foreground);
	}
}
