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
import android.support.v4.app.RemoteInput;

import org.qtproject.qt5.android.bindings.QtService;

import android.util.Log;

public class RemindmeService extends QtService {
	public enum Actions {
		ActionScheduler(20, "de.skycoder42.remindme.ActionScheduler"),
		ActionComplete(21, "de.skycoder42.remindme.ActionComplete"),
		ActionDelay(22, "de.skycoder42.remindme.ActionDelay");

		private int id;
		private String action;

		Actions(int id, String action) {
			this.id = id;
			this.action = action;
		}

		public int getId() {
			return id;
		}

		public String getAction() {
			return action;
		}
	}

	private static final String ForegroundChannelId = "foreground_channel";
	private static final String NormalChannelId = "normal_channel";
	private static final String ImportantChannelId = "important_channel";
	private static final String ErrorChannelId = "error_channel";
	private static final int ForegroundId = 40;
	private static final int NotifyId = 41;
	private static final int ErrorNotifyId = 42;
	private static final int OpenIntentId = 43;

	private static final String ExtraId = "id";
	private static final String ExtraVersion = "versionCode";

	private final IBinder _binder = new Binder();

	public static native void handleIntent(String action, String remId, int versionCode);
	public static void handleIntent(Intent intent) {
		String remId = intent.getStringExtra(ExtraId);
		int versionCode = intent.getIntExtra(ExtraVersion, 0);
		handleIntent(intent.getAction(), remId, versionCode);
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

		startForeground(ForegroundId, builder.build());
		handleIntent(intent);

		return result;
	}

	public void completeAction() {
		stopForeground(STOP_FOREGROUND_REMOVE);
		stopService(new Intent(this, RemindmeService.class));//Stop myself
	}

	public void createSchedule(String remId, int versionCode, boolean important, long triggerAt) {
		PendingIntent pending = createPending(Actions.ActionScheduler, remId, versionCode);
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		if(important)
			manager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, triggerAt, pending);
		else
			manager.setWindow(AlarmManager.RTC_WAKEUP, triggerAt, 5 * 60 * 1000, pending);//max 5 min window
	}

	public void cancelSchedule(String remId) {
		AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
		manager.cancel(createPending(Actions.ActionScheduler, remId, 0));
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

	public void notify(String remId, int versionCode, boolean important, String text) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, important ? ImportantChannelId : NormalChannelId)
			.setContentTitle(important ? "Important Reminder" : "Reminder")
			.setContentText(text)
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setOnlyAlertOnce(true)
			.setAutoCancel(true)
			.setShowWhen(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER)
			.setGroup(important ? "important" : "normal")
			.setContentIntent(RemindmeActivity.createPending(this, RemindmeActivity.Actions.ActionOpen, null, 0))
			.setDeleteIntent(createPending(Actions.ActionDelay, remId, versionCode))
			.addAction(R.drawable.ic_notification, "Complete", createPending(Actions.ActionComplete, remId, versionCode))
			.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_notification, "Snooze", createPending(Actions.ActionComplete, remId, versionCode))
				.addRemoteInput(new RemoteInput.Builder("snoozeTime")
					.setLabel("Enter a snooze time")
					.setAllowFreeFormInput(true)
					.build())
				.build());

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
			builder.setDefaults(NotificationCompat.DEFAULT_ALL)
				.setPriority(important ? NotificationCompat.PRIORITY_MAX : NotificationCompat.PRIORITY_DEFAULT);
		}

		Notification notification = builder.build();
		if(important)
			notification.flags |= NotificationCompat.FLAG_INSISTENT;

		manager.notify(remId, NotifyId, notification);
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

	private PendingIntent createPending(Actions action, String remId, int versionCode) 	{
		Uri uri = null;
		if(remId != null) {
			uri = new Uri.Builder()
				.scheme("remindme")
				.path("/service/" + remId)
				.build();
		}

		Intent intent = new Intent(action.getAction(), uri, this, RemindmeService.class);
		if (remId != null) {
			intent.putExtra(ExtraId, remId);
			intent.putExtra(ExtraVersion, versionCode);
		}
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			return PendingIntent.getService(this, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			return PendingIntent.getForegroundService(this, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
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
