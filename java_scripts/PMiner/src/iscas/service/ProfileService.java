package iscas.service;

import java.util.Date;
import java.util.List;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import iscas.entity.Profile;;

public interface ProfileService {
    /**
     * 根据姓名查找符合的人员信息
     * @param name 查询姓名
     * @return List<Profile> 人员信息列表
     */
	public List<Profile> findProfiles(String name);
	
	/**
	 * 根据id查找，返回结果唯一
	 * @param id
	 * @return Profile id对应的唯一人员信息
	 */
	public Profile findProfile(String id);
	
	/**
	 * 保存Profile到数据库中，对于重复id的进行覆盖
	 * @param profile
	 */
	public void saveProfile(Profile profile);
	
	/**
	 * 根据id从数据库中删除该记录
	 * @param id
	 * @return boolean 删除成功返回true，否则返回false
	 */
	public boolean deleteProfile(String id);
	
	/**
	 * 查找该id人物的各类关系信息（同乡，同事，同学）
	 * @param id
	 * @return JSONObject json形式表示的关系信息
	 */
	public JSONArray findLocMate(String id);
	
	public JSONArray findColleague(String id);
	
	public JSONArray findRankColleague(String id);
	
	public JSONArray findPreviousSchoolMate(String id);
	
	public JSONArray findCurrentSchoolMate(String id);
	
	public JSONArray findAllSchoolMate(String id);
	
	public JSONObject findAllRelation(String id);
	
	/**
	 * 查找各类统计信息
	 * 
	 * 
	 */
	
	/**
	 * 修改档案数据
	 */
	public boolean setLevel(String id, int level);
	
	/**
	 * 辅助方法，查找并返回所有人员的人名列表
	 */
	public JSONArray findAllNameList();
	
	public JSONArray findRankCount();
	
	public JSONObject countByWorkProvince(String province); // 根据省份名称查找该省各档位履历的数量
	
	public long countByCurrentOfficeRank(String rank);
	
	public JSONArray findByWorkProvince(String province);
	
	public long countByWorkProvinceAndTime(String province, Date start, Date end);
	
	public JSONArray findByWorkProvinceAndTime(String province, Date start, Date end);
}
