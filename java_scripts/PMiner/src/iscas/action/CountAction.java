package iscas.action;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import iscas.service.ProfileService;

import javax.annotation.Resource;
import javax.servlet.ServletRequest;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.struts2.ServletActionContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller
@Scope("prototype")
public class CountAction extends ActionSupport {
	
	private static final long serialVersionUID = -3811138606072489769L;
	@SuppressWarnings("unused")
	private static final Logger logger = LoggerFactory.getLogger(OfficeRankAction.class);
	
	@Resource
	private ProfileService profileService;

	private JSONObject jsonObjectInfo; //用于存储各种类别的json信息
	private JSONArray jsonArrayInfo;   //用于存储各种类别的json信息
	
	private String province;
	
	public JSONObject getJsonObjectInfo() {
		return jsonObjectInfo;
	}

	public void setJsonObjectInfo(JSONObject jsonObjectInfo) {
		this.jsonObjectInfo = jsonObjectInfo;
	}

	public JSONArray getJsonArrayInfo() {
		return jsonArrayInfo;
	}

	public void setJsonArrayInfo(JSONArray jsonArrayInfo) {
		this.jsonArrayInfo = jsonArrayInfo;
	}

	public String getProvince() {
		return province;
	}

	public void setProvince(String province) {
		this.province = province;
	}

	// 统计类所使用的Action，主要与index.jsp交互数据
	// 搜索人名时的自动完成使用的全部人名列表
	public String findAllNameList()	{
		jsonArrayInfo = profileService.findAllNameList();
		return SUCCESS;
	}
	
	// 获取各个级别的人员数量统计信息
	public String findRankCount() {
		jsonArrayInfo = profileService.findRankCount();
		return SUCCESS;
	}
	
	// 统计action
	// 获取各省的各档案级别的人员数量
	public String getProvinceData() {
		String provinceStr = "山东,江苏,安徽,浙江,福建,上海,广东,广西,海南,湖北,湖南,河南,江西,北京,天津,河北,山西,内蒙古,宁夏,新疆,青海,陕西,甘肃,四川,云南,贵州,西藏,重庆,辽宁,吉林,黑龙江,台湾,香港,澳门";
		
		ArrayList<String> list = new ArrayList<String>();
		for(String temp:provinceStr.split(",")){	
			list.add(temp);
		}
		
		jsonObjectInfo = new JSONObject();
		
		for (String provinceName : list) {
			JSONObject obj = profileService.countByWorkProvince(provinceName);
			jsonObjectInfo.put(provinceName, obj);
		}
		return SUCCESS;
	}
	
	// 统计全部要档案中，当前工作记录中各个级别的人员数量
	public String getCurrentRankData() {
		jsonObjectInfo = new JSONObject();
		for (int rankNum = 5; rankNum <= 10; rankNum++) {
			String rank = String.valueOf(rankNum);
			long count = profileService.countByCurrentOfficeRank(rank);
			jsonObjectInfo.put(rankNum, String.valueOf(count));
		}
		return SUCCESS;
	}
	
	public String searchByProvince() {
		try {
			province = URLDecoder.decode(province, "utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		ServletRequest request = ServletActionContext.getRequest();
		JSONArray array = profileService.findByWorkProvince(province);
		
		@SuppressWarnings("unchecked")
		List<JSONObject> result = JSONArray.toList(array, JSONObject.class);
		if (result.size() == 0)
			return NONE;
		else {
			request.setAttribute("nameList", result);
			return SUCCESS;
		}
	}
	
	
	public String getProvinceNewProfileData() {
		String provinceStr = "山东,江苏,安徽,浙江,福建,上海,广东,广西,海南,湖北,湖南,河南,江西,北京,天津,河北,山西,内蒙古,宁夏,新疆,青海,陕西,甘肃,四川,云南,贵州,西藏,重庆,辽宁,吉林,黑龙江,台湾,香港,澳门";
		
		ArrayList<String> list = new ArrayList<String>();
		for(String temp:provinceStr.split(",")){	
			list.add(temp);
		}
		
		jsonObjectInfo = new JSONObject();
		
		Calendar calendar = Calendar.getInstance();
		calendar.add(Calendar.HOUR_OF_DAY, -2); // 过去2小时的时间内
		Date start = calendar.getTime();
		
		for (String provinceName : list) {
			long count = profileService.countByWorkProvinceAndTime(provinceName, start, new Date());
			if (count != 0)
				jsonObjectInfo.put(provinceName, count);
		}
		return SUCCESS;
	}
	
	public String searchByProvinceAndTime() {
		try {
			province = URLDecoder.decode(province, "utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		Calendar calendar = Calendar.getInstance();
		calendar.add(Calendar.HOUR_OF_DAY, -2); // 过去2小时的时间内
		Date start = calendar.getTime();
		
		ServletRequest request = ServletActionContext.getRequest();
		JSONArray array = profileService.findByWorkProvinceAndTime(province, start, new Date());
		
		@SuppressWarnings("unchecked")
		List<JSONObject> result = JSONArray.toList(array, JSONObject.class);
		if (result.size() == 0)
			return NONE;
		else {
			request.setAttribute("nameList", result);
			return SUCCESS;
		}
	}
	
}
