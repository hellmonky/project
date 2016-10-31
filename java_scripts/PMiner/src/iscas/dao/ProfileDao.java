package iscas.dao;

import java.util.Date;
import java.util.List;

import net.sf.json.JSONArray;
import iscas.entity.Profile;

public interface ProfileDao {
	
	public long getCount();
	
	public Profile findById(String id);
	
	public List<Profile> findByName(String name);
	
	public List<Profile> findByKey(String key, String value);
	
	public List<Profile> findByKeyStartsWith(String key, String value);
	
	public long countByKey(String key, Object value); // 数量查询，多用于数据统计
	
	public int updateById(String id, String key, Object value);
	
	public String saveOrModify(Profile profile);   // 保存或覆盖记录
	
	public boolean deleteProfile(String id); // 删除对应id的一条记录
	
	public JSONArray findLocMate(String province); 	// 查找同乡，返回json字符串信息
	
	public JSONArray findColleague(String unitName);	// 查找同事， 返回json字符串信息
	
	public JSONArray findSchoolMate(String universityName);	// 查找同学，返回json字符串信息
	
	public JSONArray findAllNames(); // 获取所有用户的姓名列表
	
	public JSONArray findByWorkProvince(String provinceOrCity, String provinceName); // 根据当前工作的省市信息获取所有人的id和姓名，市主要针对直辖市的情况
	
	public long countByWorkProvinceAndLevel(String provinceOrCity, String provinceName, int level);
	
	public List<Profile> findByTime(Date start, Date end);
	
	public long countByWorkProvinceAndTime(String provinceOrCity, String provinceName, Date start, Date end);
	
	public List<Profile> findByWorkProvinceAndTime(String provinceOrCity, String provinceName, Date start, Date end);	
}
