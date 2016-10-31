package iscas.action;

import iscas.entity.Profile;
import iscas.service.ProfileService;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import javax.annotation.Resource;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;
 
@Controller
@Scope("prototype")
public class FileDisplay extends ActionSupport {
	
	private static final long serialVersionUID = 7453458422120576853L;
	@SuppressWarnings("unused")
	private static final Logger logger = LoggerFactory.getLogger(ProfileAction.class);
	
	private String type = "image/jpeg";
    private String id;
    private InputStream stream;
	
	@Resource
	private ProfileService profileService;

    public String execute() throws Exception {
    	Profile profile = profileService.findProfile(id);
        stream = new ByteArrayInputStream(profile.getImage());
        return SUCCESS;
    }

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public InputStream getStream() {
		return stream;
	}

	public void setStream(InputStream stream) {
		this.stream = stream;
	}
}
