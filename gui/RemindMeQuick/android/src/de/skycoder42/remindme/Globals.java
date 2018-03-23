package de.skycoder42.remindme;

import android.os.Build;

import android.content.Context;
import android.content.Intent;

import android.net.Uri;

import android.app.PendingIntent;

public class Globals {
	public static enum Actions {
		ActionScheduler(21, "de.skycoder42.remindme.ActionScheduler")/*,
		ActionComplete(22, "de.skycoder42.remindme.ActionComplete"),
		ActionDismiss(23, "de.skycoder42.remindme.ActionDismiss"),
		ActionSnooze(24, "de.skycoder42.remindme.ActionSnooze")*/;

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

	public static final String NormalChannelId = "de.skycoder42.remindme.channel.normal";
	public static final String ImportantChannelId = "de.skycoder42.remindme.channel.important";
	public static final String ErrorChannelId = "de.skycoder42.remindme.channel.error";
	public static final String ForegroundChannelId = "de.skycoder42.remindme.channel.foreground";

	private static final String ExtraId = "id";
	private static final String ExtraVersion = "versionCode";
	private static final String ExtraImportant = "important";

	public static PendingIntent createPending(Context context, Actions action, String remId, int versionCode) {
		return createPending(context, action, remId, versionCode, false);
	}

	public static PendingIntent createPending(Context context, Actions action, String remId, int versionCode, boolean important) {
		Uri uri = null;
		if(remId != null) {
			uri = new Uri.Builder()
				.scheme("remindme")
				.path("/service/" + remId)
				.build();
		}

		Intent intent = new Intent(action.getAction(), uri, context, RemindmeService.class);
		if (remId != null) {
			intent.putExtra(ExtraId, remId);
			intent.putExtra(ExtraVersion, versionCode);
			intent.putExtra(ExtraImportant, important);
		}
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O)
			return PendingIntent.getService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			return PendingIntent.getForegroundService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
	}
}
