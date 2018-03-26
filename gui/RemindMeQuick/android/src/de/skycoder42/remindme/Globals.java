package de.skycoder42.remindme;

import android.os.Build;

import android.content.Context;
import android.content.Intent;

import android.net.Uri;

import android.app.PendingIntent;

import android.graphics.Color;

public class Globals {
	public static enum Actions {
		ActionScheduler(21, "de.skycoder42.remindme.Action.Scheduler"),
		ActionComplete(22, "de.skycoder42.remindme.Action.Complete"),
		ActionSnooze(23, "de.skycoder42.remindme.Action.Snooze");

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

	// notifications channels
	public static final String NormalChannelId = "de.skycoder42.remindme.channel.normal";
	public static final String ImportantChannelId = "de.skycoder42.remindme.channel.important";
	public static final String ErrorChannelId = "de.skycoder42.remindme.channel.error";
	public static final String ForegroundChannelId = "de.skycoder42.remindme.channel.foreground";

	// intent extras
	public static final String ExtraId = "id";
	public static final String ExtraVersion = "versionCode";
	public static final String ExtraSnoozeTime = "snoozeTime";

	// intent ids
	public static final int OpenActivityId = 31;

	// notifications IDs
	public static final int ForegroundId = 41;
	public static final int NotifyId = 42;
	public static final int ErrorNotifyId = 43;

	// colors
	public static final int ForegroundColor = 0xFF673AB7;
	public static final int NormalColor = 0xFF673AB7;
	public static final int ImportantColor = 0xFFEF6C00;
	public static final int ErrorColor = Color.RED;

	public static boolean isOreo() {
		return Build.VERSION.SDK_INT >= Build.VERSION_CODES.O;
	}

	public static PendingIntent createPending(Context context, Actions action, String remId, int versionCode) {
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
		}
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);

		if (isOreo())
			return PendingIntent.getForegroundService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
		else
			return PendingIntent.getService(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
	}
}
