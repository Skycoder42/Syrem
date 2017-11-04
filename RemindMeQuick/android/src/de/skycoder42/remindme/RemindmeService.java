package de.skycoder42.remindme;

import java.util.Map;
import java.util.HashMap;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.os.Binder;
import android.os.Bundle;
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

public class RemindmeService extends QtService {
	public enum Actions {
		ActionScheduler(21, "de.skycoder42.remindme.ActionScheduler"),
		ActionComplete(22, "de.skycoder42.remindme.ActionComplete"),
		ActionDismiss(23, "de.skycoder42.remindme.ActionDismiss"),
		ActionSnooze(24, "de.skycoder42.remindme.ActionSnooze");

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

	private static final int AutoSyncId = 20;

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
	private static final String ExtraSnoozeTime = "snoozeTime";
	private static final String ExtraImportant = "important";

	private final IBinder _binder = new Binder();
	private final Map<String, Boolean> _impMap = new HashMap<>();

	public static native void handleIntent(String action, String remId, int versionCode, String resultExtra);
	public void handleIntent(Intent intent) {
		String remId = intent.getStringExtra(ExtraId);
		int versionCode = intent.getIntExtra(ExtraVersion, 0);
		if(remId != null)
			_impMap.put(remId, intent.getBooleanExtra(ExtraImportant, false));

		String resultExtra = null;
		if(intent.getAction() == Actions.ActionSnooze.getAction()) {
			Bundle remoteInput = RemoteInput.getResultsFromIntent(intent);
			if(remoteInput != null)
				resultExtra = remoteInput.getCharSequence(ExtraSnoozeTime).toString();
		}

		handleIntent(intent.getAction(), remId, versionCode, resultExtra);
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
			.setContentTitle(getString(R.string.svc_name))
			.setContentText(getString(R.string.svc_desc))
			.setContentInfo(getString(R.string.app_name))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setLocalOnly(true)
			.setOngoing(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			builder.setPriority(NotificationCompat.PRIORITY_MIN);

		startForeground(ForegroundId, builder.build());
		if(intent != null)
			handleIntent(intent);

		return result;
	}

	public void completeAction() {
		stopForeground(true);
		stopService(new Intent(this, RemindmeService.class));//Stop myself
	}

	public void scheduleAutoCheck() {
		Intent intent = new Intent(Actions.ActionScheduler.getAction(), null, this, RemindmeService.class);
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);

		PendingIntent pending = null;
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			pending = PendingIntent.getService(this, AutoSyncId, intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			pending = PendingIntent.getForegroundService(this, AutoSyncId, intent, PendingIntent.FLAG_UPDATE_CURRENT);

		AlarmManager alarm = (AlarmManager)getSystemService(Context.ALARM_SERVICE);
		alarm.setRepeating(AlarmManager.RTC_WAKEUP,
			System.currentTimeMillis() + AlarmManager.INTERVAL_HOUR,
			AlarmManager.INTERVAL_HOUR,
			pending);
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

	public void ensureCanActive() {
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
			NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
			manager.cancelAll();
		}
	}

	public String[] activeNotifications() {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		StatusBarNotification[] allNots = null;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
			allNots = manager.getActiveNotifications();
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
			.setContentTitle(getString(important ? R.string.not_imp_name : R.string.not_nrm_name))
			.setContentText(text)
			.setContentInfo(getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setOnlyAlertOnce(true)
			.setAutoCancel(false)
			.setShowWhen(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER)
			.setGroup(important ? "important" : "normal")
			.setContentIntent(RemindmeActivity.createPending(this, RemindmeActivity.Actions.ActionOpen, null, 0))
			.setDeleteIntent(createPending(Actions.ActionDismiss, remId, versionCode))
			.addAction(R.drawable.ic_check_black_24dp, getString(R.string.not_complete), createPending(Actions.ActionComplete, remId, versionCode))
			.addAction(R.drawable.ic_snooze_black_24dp, getString(R.string.not_snooze), RemindmeActivity.createPending(this, RemindmeActivity.Actions.ActionSnooze, remId, versionCode));

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
			builder.setDefaults(NotificationCompat.DEFAULT_ALL)
				.setPriority(important ? NotificationCompat.PRIORITY_MAX : NotificationCompat.PRIORITY_DEFAULT);
		}

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
			builder.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_snooze_black_24dp, getString(R.string.not_snooze_inline), createPending(Actions.ActionSnooze, remId, versionCode, important))
				.addRemoteInput(new RemoteInput.Builder(ExtraSnoozeTime)
					.setLabel(getString(R.string.not_snooze_label))
					.setAllowFreeFormInput(true)
					.build())
				.build());
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
			.setContentTitle(getString(R.string.not_err_name))
			.setContentText(text)
			.setContentInfo(getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setAutoCancel(true)
			.setCategory(NotificationCompat.CATEGORY_ERROR);

		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			builder.setDefaults(NotificationCompat.DEFAULT_ALL);

		manager.notify(ErrorNotifyId, builder.build());
	}

	public void cancelNotify(String id) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		manager.cancel(id, NotifyId);
	}

	public void notifyReminderError(String remId, int versionCode, String errorText) {
		Boolean important = _impMap.remove(remId);
		if(important == null)
			important = false;
		notify(remId, versionCode, important, errorText);
	}

	private PendingIntent createPending(Actions action, String remId, int versionCode) {
		return createPending(action, remId, versionCode, false);
	}
	private PendingIntent createPending(Actions action, String remId, int versionCode, boolean important) {
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
			intent.putExtra(ExtraImportant, important);
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
			getString(R.string.channel_nrm_name),
			NotificationManager.IMPORTANCE_DEFAULT);
		normal.setDescription(getString(R.string.channel_nrm_desc));
		normal.enableLights(true);
		normal.setLightColor(Color.GREEN);
		normal.enableVibration(true);
		normal.setShowBadge(true);
		manager.createNotificationChannel(normal);

		//create important channel
		NotificationChannel important = new NotificationChannel(ImportantChannelId,
			getString(R.string.channel_imp_name),
			NotificationManager.IMPORTANCE_HIGH);
		important.setDescription(getString(R.string.channel_imp_desc));
		important.enableLights(true);
		important.setLightColor(Color.RED);
		important.enableVibration(true);
		important.setShowBadge(true);
		important.setBypassDnd(true);
		manager.createNotificationChannel(important);

		//create error channel
		NotificationChannel error = new NotificationChannel(ErrorChannelId,
			getString(R.string.channel_err_name),
			NotificationManager.IMPORTANCE_DEFAULT);
		error.setDescription(getString(R.string.channel_err_desc));
		error.enableLights(true);
		error.setLightColor(Color.RED);
		error.enableVibration(true);
		error.setShowBadge(true);
		manager.createNotificationChannel(error);

		//create foreground channel
		NotificationChannel foreground = new NotificationChannel(ForegroundChannelId,
			getString(R.string.channel_svc_name),
			NotificationManager.IMPORTANCE_MIN);
		foreground.setDescription(getString(R.string.channel_svc_desc));
		foreground.enableLights(false);
		foreground.enableVibration(false);
		foreground.setShowBadge(false);
		manager.createNotificationChannel(foreground);
	}
}
