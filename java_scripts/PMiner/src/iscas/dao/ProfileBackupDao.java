package iscas.dao;

import iscas.entity.Profile;

public interface ProfileBackupDao {
	
	public String saveOrModify(Profile profile); // 备份保存已经修改过的人员履历数据

}
