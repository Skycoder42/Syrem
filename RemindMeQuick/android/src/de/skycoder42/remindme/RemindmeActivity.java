package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.app.PendingIntent;
import android.os.IBinder;
import android.os.Bundle;
import android.net.Uri;
import org.qtproject.qt5.android.bindings.QtActivity;

public class RemindmeActivity extends QtActivity {
	public enum Actions {
		ActionOpen(10, "de.skycoder42.remindme.ActionOpen"),
		ActionSnooze(11, "de.skycoder42.remindme.ActionSnooze");

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

	private static final String ExtraId = "id";
	private static final String ExtraVersion = "versionCode";

	private boolean _isBound;

	private ServiceConnection _connection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName className, IBinder service) {
			_isBound = true;
		}

		@Override
		public void onServiceDisconnected(ComponentName className) {
			_isBound = false;
		}
	};

	public static native void handleIntent(String action, String remId, int versionCode);
	public void handleIntent(Intent intent) {
		String remId = intent.getStringExtra(ExtraId);
		int versionCode = intent.getIntExtra(ExtraVersion, 0);
		handleIntent(intent.getAction(), remId, versionCode);
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(this, RemindmeService.class);
		bindService(intent, _connection, Context.BIND_AUTO_CREATE);
	}

	@Override
	protected void onStart() {
		super.onStart();
		Intent intent = getIntent();
		if(intent != null)
			handleIntent(intent);
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		if(intent != null)
			handleIntent(intent);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (_isBound) {
			unbindService(_connection);
			_isBound = false;
		}
	}

	public static PendingIntent createPending(Context context, Actions action, String remId, int versionCode) 	{
		Uri uri = null;
		if(remId != null) {
			uri = new Uri.Builder()
				.scheme("remindme")
				.path("/activity/" + remId)
				.build();
		}

		Intent intent = new Intent(action.getAction(), uri, context, RemindmeActivity.class);
		if (remId != null) {
			intent.putExtra(ExtraId, remId);
			intent.putExtra(ExtraVersion, versionCode);
		}
		intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
		return PendingIntent.getActivity(context, action.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
	}
}
