package com.fro.wifi_fancase;

import android.util.Log;

import java.io.IOException;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.os.AsyncTask;

public class MainActivity extends Activity {

	private static String TAG = "MainActivity";

	private EditText ip_et;
	private EditText port_et;
	private ToggleButton connect_tb;
	private TextView error_tv;
	private ToggleButton fan_tb;

	private Context context;
	private ConnectTask connectTask;
	private ControlTask controlTask;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		// 控件
		bindView();
		// 数据
		initData();
		// 事件
		initEvent();

	}

	/**
	 * 初始化数据
	 */
	private void initData() {
		context = this;
		error_tv.setText("请点击连接！");
		ip_et.setText(Constant.IP);
		port_et.setText(String.valueOf(Constant.port));
	}

	/**
	 * 绑定控件
	 */
	private void bindView() {
		ip_et = (EditText) findViewById(R.id.ip_et);
		port_et = (EditText) findViewById(R.id.port_et);
		connect_tb = (ToggleButton) findViewById(R.id.connect_tb);
		error_tv = (TextView) findViewById(R.id.error_tv);
		fan_tb = (ToggleButton) findViewById(R.id.fan_tb);
	}

	/**
	 * 按钮监听
	 */
	private void initEvent() {
		// 连接
		connect_tb.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (isChecked) {
					// 获取IP和端口
					Constant.IP = ip_et.getText().toString().trim();
					Constant.port = Integer.parseInt(port_et.getText().toString().trim());
//					Log.i(TAG, "ip=" + Constant.IP + "   port=" + Constant.port);
					// 开启任务
					connectTask = new ConnectTask(context, error_tv);
					connectTask.execute();
				} else {
					try {
						// 取消任务
						if (connectTask != null && connectTask.getStatus() == AsyncTask.Status.RUNNING) {
							Thread.sleep(500);
							// 如果Task还在运行，则先取消它
							connectTask.cancel(true);
							error_tv.setText("请点击连接！");
						}
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		});
		
		//控制
		fan_tb.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if(connectTask.getSTATU()){
					if (isChecked) {
						// 打开
						controlTask = new ControlTask(context, 
								connectTask.getInputStream(), connectTask.getOutputStream(), Constant.FAN_ON);
						controlTask.execute();
					} else {
						//关闭
						controlTask = new ControlTask(context, 
								connectTask.getInputStream(), connectTask.getOutputStream(), Constant.FAN_OFF);
						controlTask.execute();
					}
				}else{
					Toast.makeText(context, "请先连接再操作！", Toast.LENGTH_SHORT).show();
				}
			}
		});
	}

	@Override
	public void finish() {
		super.finish();
		try {
			// 取消任务
			if (connectTask != null && connectTask.getStatus() == AsyncTask.Status.RUNNING) {
				Thread.sleep(500);
				// 如果Task还在运行，则先取消它
				connectTask.cancel(true);
				connectTask.getmSocket().close();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
