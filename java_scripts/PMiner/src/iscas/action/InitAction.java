package iscas.action;

import javax.annotation.PostConstruct;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller
public class InitAction extends ActionSupport {
	
	private static final long serialVersionUID = 9203311333385257785L;
	private static final Logger logger = LoggerFactory.getLogger(InitAction.class);
	
	@PostConstruct
	public void init() {
		@SuppressWarnings("unused")
		CallThreadTimer timeController = new CallThreadTimer(7200); // seconds timer
		logger.info("Web Crawl Thread Start!");
	}
}
