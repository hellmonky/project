package iscas.action;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.Date;
import java.util.List;

import iscas.entity.Profile;
import iscas.entity.OfficeRecord;
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

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.opensymphony.xwork2.ActionSupport;

@Controller
@Scope("prototype")
public class ProfileAction extends ActionSupport {
	
	private static final long serialVersionUID = 3961867064193581798L;
	@SuppressWarnings("unused")
	private static final Logger logger = LoggerFactory.getLogger(ProfileAction.class);
	
	@Resource
	private ProfileService profileService;
	
	private String name;
	private String id;
	private String jsonProfile;
	private Profile profile;
	private List<Profile> profileList;
	
	private JSONObject jsonObjectInfo; //用于存储各种类别的json信息，例如查询到的同事信息、同学信息、同乡信息等
	private JSONArray jsonArrayInfo;   //用于存储各种类别的json信息，例如查询到的同事信息、同学信息、同乡信息等
	
	private int level;
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}
	
	public String getId() {
		return id;
	}
	
	public void setId(String id) {
		this.id = id;
	}
	
	public String getJsonProfile() {
		return jsonProfile;
	}

	public void setJsonProfile(String jsonProfile) {
		this.jsonProfile = jsonProfile;
	}

	public List<Profile> getProfileList() {
		return profileList;
	}
	
	public Profile getProfile() {
		return profile;
	}

	public void setProfile(Profile profile) {
		this.profile = profile;
	}

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

	public int getLevel() {
		return level;
	}

	public void setLevel(int level) {
		this.level = level;
	}

	// action methods below
	// 基本的查找action，根据id查询人物简历信息
	public String find() {
		profile = profileService.findProfile(id);
        return SUCCESS;
	}

	// 保存action
	public String save() {
		// 保存基于JSON格式的人员数据
		
	    //JSONObject jsonObject = JSONObject.fromObject(jsonProfile);
	    //Profile pf = (Profile) JSONObject.toBean(jsonObject, Profile.class); // json-lib 无法完成此类复杂JavaBean的转换
	    
		// 使用jackson json类库读取json并转换为JavaBean Profile对象
		ObjectMapper mapper = new ObjectMapper();
	    Profile pf = null;
		
	    try {
			pf = mapper.readValue(jsonProfile, Profile.class);
		} catch (JsonParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (JsonMappingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	    // 此时转换的pf缺少一部分信息，主要是latestOfficeRecord, image, timestamp
	    // 因此需要借助原存储的一些信息
	    profile = profileService.findProfile(pf.getId());
	    
	    // 设置pf中缺失的数据
	    pf.setImage(profile.getImage());
		// 处理该人的工作记录，找出最新的工作记录
		List<OfficeRecord> officeRecordList = pf.getOfficeRecord();
		if (officeRecordList != null && officeRecordList.size() != 0) {
			OfficeRecord item = officeRecordList.get(officeRecordList.size() -1);
			pf.setLatestOfficeRecord(item);
		}
		// 设置入库时间戳
		pf.setTimestamp(new Date());
		profileService.saveProfile(pf);
		return SUCCESS;
	}
	
	// 删除action
	public String delete() {
		if (profileService.deleteProfile(id))
			return SUCCESS;
		else
			return ERROR;
	}

	// 基本的查找action，根据姓名查找并处理重名的情况
	public String searchByName() {
		// 根据姓名查找
		// 这里考虑了数据库中重名的情况
		try {
			name = URLDecoder.decode(name, "utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		ServletRequest request = ServletActionContext.getRequest();
		try {
			request.setCharacterEncoding("utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		profileList = profileService.findProfiles(name);
		if (profileList.size() == 1) {
			// 如果查询结果中是唯一一人，直接转到该人的详细信息页面profile.jsp
			request.setAttribute("profile", profileList.get(0));
			return SUCCESS;
		} else if (profileList.size() == 0)
			return NONE; // 没找到结果，跳转到empty.jsp
		else {
			// 查询结果是多人，跳转到multiple.jsp，可以展示多人的基本信息，再由用户选择
			request.setAttribute("profileList", profileList);
			return "multiple";
		}
	}
	
	// 基本的查找action，根据唯一id查找该人物的履历信息
	public String searchById() {
		// 根据姓名查找
		// 这里考虑了数据库中重名的情况
		ServletRequest request = ServletActionContext.getRequest();
		try {
			request.setCharacterEncoding("utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		profile = profileService.findProfile(id);
		if (profile == null)
			return NONE;
		else {
			request.setAttribute("profile", profile);
			return SUCCESS;
		}
	}
	
	// 综合查询action，根据关键字进行综合查询
	public String searchByKeyword() {
		// name -> keyword
		try {
			name = URLDecoder.decode(name, "utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		ServletRequest request = ServletActionContext.getRequest();
		try {
			request.setCharacterEncoding("utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		profileList = profileService.findProfiles(name); // TODO
		request.setAttribute("profileList", profileList);
		return SUCCESS;
	}
	
	// 查找一些基本信息，以json格式返回，主要用于compare.jsp的Ajax异步加载
	public String findBasicInfo() {
		// 由于compare.jsp的特殊情况，这里只能使用姓名查找
		// 不考虑重名的情况，只取查找到的第一个结果
		profile = profileService.findProfile(id);
		
		jsonObjectInfo = new JSONObject();
		jsonObjectInfo.put("id", profile.getId());
		jsonObjectInfo.put("name", profile.getName());
		jsonObjectInfo.put("birthDate", profile.getBirthDate());
		jsonObjectInfo.put("xmlPath", profile.getXmlPath());
		jsonObjectInfo.put("rawText", profile.getRawText());
		jsonObjectInfo.put("timestamp", profile.getTimestamp().getTime());
		
		return SUCCESS;
	}
	
	// 查找各类关系的几个函数，同乡，同事（2个函数），同学（2个函数）关系
	public String findLocMate() {
		jsonArrayInfo = profileService.findLocMate(id);
		return SUCCESS;
	}
	
	public String findColleague() {
		jsonArrayInfo = profileService.findColleague(id);
		return SUCCESS;
	}
	
	public String findRankColleague() {
		jsonArrayInfo = profileService.findRankColleague(id);
		return SUCCESS;
	}
	
	public String findCurrentSchoolMate() {
		jsonArrayInfo = profileService.findCurrentSchoolMate(id);
		return SUCCESS;
	}
	
	public String findPreviousSchoolMate() {
		jsonArrayInfo = profileService.findPreviousSchoolMate(id);
		return SUCCESS;
	}
	
	public String findAllRelationship() {
		jsonObjectInfo = profileService.findAllRelation(id);
		return SUCCESS;
	}
	
	// 修改action
	// 修改档案分级
	public String updateLevel() {
		profileService.setLevel(id, level);
		return SUCCESS;
	}
}
