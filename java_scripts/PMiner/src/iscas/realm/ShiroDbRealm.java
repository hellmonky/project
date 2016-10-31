/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
package iscas.realm;

import iscas.entity.User;
import iscas.service.UserService;

import org.apache.commons.lang.StringUtils;
import org.apache.shiro.authc.*;
import org.apache.shiro.authz.AuthorizationInfo;
import org.apache.shiro.authz.SimpleAuthorizationInfo;
import org.apache.shiro.realm.AuthorizingRealm;
import org.apache.shiro.subject.PrincipalCollection;
import org.apache.shiro.util.ByteSource;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


/**
 * Source:http://shiro.apache.org/java-authentication-guide.html
 * 
 * Subject 		Security specific user 'view' of an application user. It can be a human being, a third-party process, 
 * 					a server connecting to you application application, or even a cron job. Basically, it is anything or 
 * 					anyone communicating with your application.
 * 
 * Principals	A subjects identifying attributes. First name, last name, social security number, username
 * 
 * Credentials	secret data that are used to verify identities. Passwords, Biometric data, x509 certificates,
 * 
 * Realms		Security specific DAO, data access object, software component that talkts to a backend data source. 
 * 				If you have usernames and password in LDAP, then you would have an LDAP Realm that would communicate 
 * 				with LDAP. The idea is that you would use a realm per back-end data source and Shiro would know how 
 * 				to coordinate with these realms together to do what you have to do.
 *
 */
public class ShiroDbRealm extends AuthorizingRealm {
	
    private static final Logger logger = LoggerFactory.getLogger(ShiroDbRealm.class);
    
    public ShiroDbRealm() {
        super();    //To change body of overridden methods use File | Settings | File Templates.
    }
    
    private UserService userService;

    public void setUserService(UserService userService) {
        this.userService = userService;
    }
	/**
	 * 授权查询回调函数, 进行鉴权但缓存中无用户的授权信息时调用.
	 */
	@Override
	protected AuthorizationInfo doGetAuthorizationInfo(PrincipalCollection principals) {
		//得到 doGetAuthenticationInfo 方法中传入的凭证
		String userName = (String)principals.getPrimaryPrincipal();
		
		if(StringUtils.equals("admin", userName)) {
			
			SimpleAuthorizationInfo info = new SimpleAuthorizationInfo();
			
			//这个确定页面中<shiro:hasRole>标签的name的值
			info.addRole("admin");
			//这个就是页面中 <shiro:hasPermission> 标签的name的值
			info.addStringPermission("user:edit");
			
			return info;
		} else { // if(StringUtils.equals("guest", userName))  暂时其他所有账号都是guest权限
			SimpleAuthorizationInfo info = new SimpleAuthorizationInfo();
			
			//这个确定页面中<shiro:hasRole>标签的name的值
			info.addRole("guest");
			//这个就是页面中 <shiro:hasPermission> 标签的name的值
			info.addStringPermission("user:view");
			
			return info;
		} 
		/*else {
			return null;
		}
		*/
	}
	
	/**
	 * 认证回调函数, 登录时调用.
	 */
	@Override
	protected AuthenticationInfo doGetAuthenticationInfo(AuthenticationToken token) throws AuthenticationException {
		
		UsernamePasswordToken usernamePasswordToken = (UsernamePasswordToken)token;
		
		String username = usernamePasswordToken.getUsername();
		
		logger.info("username: {}.", username);
		
		System.out.println("=============doGetAuthenticationInfo begin ===============");
		System.out.println("username: " + username);
		System.out.print("password: ");
		System.out.println(usernamePasswordToken.getPassword());
		System.out.println("principal: " + usernamePasswordToken.getPrincipal());
		System.out.println("=============doGetAuthenticationInfo end =================");
		
		User user = userService.findByUsername(username);

        if(user == null) {
            throw new UnknownAccountException();//没找到帐号
        }

        if(Boolean.TRUE.equals(user.getLocked())) {
            throw new LockedAccountException(); //帐号锁定
        }

        //交给AuthenticatingRealm使用CredentialsMatcher进行密码匹配，如果觉得人家的不好可以自定义实现
        SimpleAuthenticationInfo authenticationInfo = new SimpleAuthenticationInfo(
                user.getUsername(), //用户名
                user.getPassword(), //密码
                ByteSource.Util.bytes(user.getCredentialsSalt()),//salt=username+salt
                getName()  //realm name
        );
        return authenticationInfo;
		
		/**
		 * Constructor that takes in a single 'primary' principal of the account, its corresponding hashed credentials, the salt used to hash the credentials, and the name of the realm to associate with the principals.
		 * This is a convenience constructor and will construct a PrincipalCollection based on the principal and realmName argument.
		 * 
		 * 
		 * Parameters:
		 * 
		 * principal - the 'primary' principal associated with the specified realm.
		 * hashedCredentials - the hashed credentials that verify the given principal.
		 * credentialsSalt - the salt used when hashing the given hashedCredentials
		 * realmName - the realm from where the principal and credentials were acquired.
		 */
        /*
		if(StringUtils.equals("admin", username)) {
			return new SimpleAuthenticationInfo("admin", "a@pminer", ByteSource.Util.bytes("admin"), getName());
		} else if(StringUtils.equals("guest", username)) {
			return new SimpleAuthenticationInfo("guest", "guest", ByteSource.Util.bytes("guest"), getName());
		}
		return null;
		*/
	}
	
    @Override
    public void clearCachedAuthorizationInfo(PrincipalCollection principals) {
        super.clearCachedAuthorizationInfo(principals);
    }

    @Override
    public void clearCachedAuthenticationInfo(PrincipalCollection principals) {
        super.clearCachedAuthenticationInfo(principals);
    }

    @Override
    public void clearCache(PrincipalCollection principals) {
        super.clearCache(principals);
    }

    public void clearAllCachedAuthorizationInfo() {
        getAuthorizationCache().clear();
    }

    public void clearAllCachedAuthenticationInfo() {
        getAuthenticationCache().clear();
    }

    public void clearAllCache() {
        clearAllCachedAuthenticationInfo();
        clearAllCachedAuthorizationInfo();
    }
	
}
