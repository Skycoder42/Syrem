package de.skycoder42.syrem;

import android.content.Context;
import android.content.Intent;

import android.app.PendingIntent;
import android.app.Notification;
import android.app.NotificationManager;

import android.graphics.BitmapFactory;
import android.graphics.Color;

import android.support.v4.app.NotificationCompat;
import android.support.v4.app.RemoteInput;

public class Notifier {
	private Context _context;

	public Notifier(Context context) {
		_context = context;
	}

	public void notify(String remId, int versionCode, boolean important, CharSequence text, boolean hasUrls, String[] choices) {
		notify(remId, versionCode, important, text, hasUrls, choices, false);
	}
	public void notify(String remId, int versionCode, boolean important, CharSequence text, boolean hasUrls, String[] choices, boolean isError) {
		NotificationManager manager = (NotificationManager) _context.getSystemService(Context.NOTIFICATION_SERVICE);

		Intent activityIntent = new Intent(_context, SyremActivity.class);
		PendingIntent activityPending = PendingIntent.getActivity(_context,
			Globals.OpenActivityId,
			activityIntent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(_context, important ? Globals.ImportantChannelId : Globals.NormalChannelId)
			.setContentTitle(_context.getString(important ? R.string.not_imp_name : R.string.not_nrm_name))
			.setContentText(text)
			.setContentInfo(_context.getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setLargeIcon(BitmapFactory.decodeResource(_context.getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(isError ? Globals.ErrorColor : (important ? Globals.ImportantColor : Globals.NormalColor))
			.setOnlyAlertOnce(true)
			.setShowWhen(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER)
			.setGroup(important ? "important" : "normal")
			.setContentIntent(activityPending)
			.addAction(R.drawable.ic_check,
					_context.getString(R.string.not_complete),
					Globals.createPending(_context, Globals.Actions.ActionComplete, remId, versionCode));

		if (Globals.isOreo()) {
			builder.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_snooze,
						_context.getString(R.string.not_snooze),
						Globals.createPending(_context, Globals.Actions.ActionSnooze, remId, versionCode))
					.addRemoteInput(new RemoteInput.Builder(Globals.ExtraSnoozeTime)
						.setLabel(_context.getString(R.string.not_snooze_label))
						.setAllowFreeFormInput(true)
						.setChoices(choices)
						.build())
					.build());
		} else {
			builder.setDefaults(NotificationCompat.DEFAULT_ALL)
				.setPriority(important ? NotificationCompat.PRIORITY_MAX : NotificationCompat.PRIORITY_DEFAULT)
				.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_snooze,
						_context.getString(R.string.not_snooze),
						activityPending)
					.setShowsUserInterface(true)
					.build());
		}

		if(hasUrls) {
			builder.addAction(R.drawable.ic_open_in_browser,
						_context.getString(R.string.not_open_url),
						Globals.createPending(_context, Globals.Actions.ActionOpenUrls, remId, versionCode));
		}

		Notification notification = builder.build();
		if(important)
			notification.flags |= NotificationCompat.FLAG_INSISTENT;

		manager.notify(remId, Globals.NotifyId, notification);
	}

	public static void cancelExplicitly(Context context, String id) {
		NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
		manager.cancel(id, Globals.NotifyId);
	}

	public void cancel(String id) {
		cancelExplicitly(_context, id);
	}

	public void cancelAll() {
		NotificationManager manager = (NotificationManager) _context.getSystemService(Context.NOTIFICATION_SERVICE);
		manager.cancelAll();
	}

	public void notifyError(String text) {
		NotificationManager manager = (NotificationManager) _context.getSystemService(Context.NOTIFICATION_SERVICE);

		Intent intent = new Intent(_context, SyremActivity.class);
		PendingIntent pending = PendingIntent.getActivity(_context,
			Globals.OpenActivityId,
			intent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(_context, Globals.ErrorChannelId)
			.setContentTitle(_context.getString(R.string.not_err_name))
			.setContentText(text)
			.setContentInfo(_context.getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(_context.getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(Color.RED)
			.setAutoCancel(true)
			.setCategory(NotificationCompat.CATEGORY_ERROR);

		if (!Globals.isOreo())
			builder.setDefaults(NotificationCompat.DEFAULT_ALL);

		manager.notify(Globals.ErrorNotifyId, builder.build());
	}
}
