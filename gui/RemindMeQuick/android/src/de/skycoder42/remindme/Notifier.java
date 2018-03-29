package de.skycoder42.remindme;

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
	private Context context;

	public Notifier(Context context) {
		this.context = context;
	}

	public void notify(String remId, int versionCode, boolean important, CharSequence text, String[] choices) {
		notify(remId, versionCode, important, text, choices, false);
	}
	public void notify(String remId, int versionCode, boolean important, CharSequence text, String[] choices, boolean isError) {
		NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

		Intent activityIntent = new Intent(context, RemindmeActivity.class);
		PendingIntent activityPending = PendingIntent.getActivity(context,
			Globals.OpenActivityId,
			activityIntent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(context, important ? Globals.ImportantChannelId : Globals.NormalChannelId)
			.setContentTitle(context.getString(important ? R.string.not_imp_name : R.string.not_nrm_name))
			.setContentText(text)
			.setContentInfo(context.getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setLargeIcon(BitmapFactory.decodeResource(context.getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(isError ? Globals.ErrorColor : (important ? Globals.ImportantColor : Globals.NormalColor))
			.setOnlyAlertOnce(true)
			.setShowWhen(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER)
			.setGroup(important ? "important" : "normal")
			.setContentIntent(activityPending)
			.addAction(R.drawable.ic_check,
					context.getString(R.string.not_complete),
					Globals.createPending(context, Globals.Actions.ActionComplete, remId, versionCode));

		if (Globals.isOreo()) {
			builder.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_snooze,
						context.getString(R.string.not_snooze),
						Globals.createPending(context, Globals.Actions.ActionSnooze, remId, versionCode))
					.addRemoteInput(new RemoteInput.Builder(Globals.ExtraSnoozeTime)
						.setLabel(context.getString(R.string.not_snooze_label))
						.setAllowFreeFormInput(true)
						.setChoices(choices)
						.build())
					.build());
		} else {
			builder.setDefaults(NotificationCompat.DEFAULT_ALL)
				.setPriority(important ? NotificationCompat.PRIORITY_MAX : NotificationCompat.PRIORITY_DEFAULT)
				.addAction(new NotificationCompat.Action.Builder(R.drawable.ic_snooze,
						context.getString(R.string.not_snooze),
						activityPending)
//					.setShowsUserInterface(true)
					.build());
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
		cancelExplicitly(context, id);
	}

	public void cancelAll() {
		NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
		manager.cancelAll();
	}

	public void notifyError(String text) {
		NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

		Intent intent = new Intent(context, RemindmeActivity.class);
		PendingIntent pending = PendingIntent.getActivity(context,
			Globals.OpenActivityId,
			intent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(context, Globals.ErrorChannelId)
			.setContentTitle(context.getString(R.string.not_err_name))
			.setContentText(text)
			.setContentInfo(context.getString(R.string.app_name))
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(text))
			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(context.getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(Color.RED)
			.setAutoCancel(true)
			.setCategory(NotificationCompat.CATEGORY_ERROR);

		if (!Globals.isOreo())
			builder.setDefaults(NotificationCompat.DEFAULT_ALL);

		manager.notify(Globals.ErrorNotifyId, builder.build());
	}
}
