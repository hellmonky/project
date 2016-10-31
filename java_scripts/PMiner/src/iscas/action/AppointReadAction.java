package iscas.action;

import iscas.entity.AppointInfo;
import iscas.service.ConfigLoad;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.annotation.PostConstruct;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller
public class AppointReadAction extends ActionSupport {
	
	private static final long serialVersionUID = 837935134841919349L;
	
	private String name;   	//参数：姓名
	private String province;//参数：省份
	
	//			name -> appointInfo
	private Map<String, AppointInfo> appointMap = null;
	
	//ajax查询时返回的JSON数据
	private JSONArray jsonNameList = null;
	private JSONArray jsonProvinceList = null;
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	public String getProvince() {
		return province;
	}

	public void setProvince(String province) {
		this.province = province;
	}

	public JSONArray getJsonNameList() {
		return jsonNameList;
	}

	public void setJsonNameList(JSONArray jsonNameList) {
		this.jsonNameList = jsonNameList;
	}

	public JSONArray getJsonProvinceList() {
		return jsonProvinceList;
	}

	public void setJsonProvinceList(JSONArray jsonProvinceList) {
		this.jsonProvinceList = jsonProvinceList;
	}
	
	@PostConstruct
	public void init() {
		try {
			readAppointDir();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public String readAppointDir() throws IOException {
		appointMap = new HashMap<String, AppointInfo>();
		
		//任免信息命名为：AppOrDisData_xxxx-mm-dd.txt
		File dir = new File(ConfigLoad.getAppointDir());
		//今天的任命信息文件命名
		Calendar c = Calendar.getInstance();
		String fileName = "AppOrDisData_" + c.get(Calendar.YEAR) + "-" + (c.get(Calendar.MONTH) + 1) + "-" + c.get(Calendar.DATE) + ".txt";
		
		String filePath = dir.getAbsolutePath() + File.separator + fileName;
		System.out.println("Read Appoint txt file: " + filePath);
		
		File appointFile = new File(filePath);
		if (appointFile.isFile() && appointFile.exists()) {
			InputStreamReader read = new InputStreamReader(new FileInputStream(
					appointFile));
			BufferedReader bufferedReader = new BufferedReader(read);
			
			String line = null;
			while ((line = bufferedReader.readLine()) != null) {
				String[] keys = line.split("@");
				
				AppointInfo info = new AppointInfo();
				int i = 0;
				info.setIndex(keys[i++]);
				info.setLocation(keys[i++]);
				info.setDate(keys[i++]);
				info.setHref(keys[i++]);
				info.setAct(keys[i++]);
				info.setName(keys[i++]);
				info.setPosition(keys[i++]);
				
				appointMap.put(info.getName(), info);
			}
			
			bufferedReader.close();
		} else {
			return ERROR;
		}
		
		return SUCCESS;
	}
	
	public String getAppointListByProvince() {
		jsonNameList = new JSONArray();
		try {
			province = URLDecoder.decode(province, "utf-8");
		} catch (UnsupportedEncodingException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		for (AppointInfo value : appointMap.values()) {
			if (value.getLocation().equals(province)) {
				JSONObject object = JSONObject.fromObject(value);
				jsonNameList.add(object);
			}
		}
		return SUCCESS;
	}
	
	public String getAppointProvinceList() {
		Set<String> set = new HashSet<String>();
		for (AppointInfo value : appointMap.values()) {
			set.add(value.getLocation());
		}
		jsonProvinceList = JSONArray.fromObject(set);
		return SUCCESS;
	}
}
