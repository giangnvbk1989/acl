#include "stdafx.h"
#include "http_service.h"

static bool http_get_default(HttpRequest&, HttpResponse& res)
{
	acl::string buf("default: hello world!\r\n");
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_get_ok(HttpRequest&, HttpResponse& res)
{
	acl::string buf("ok: hello world!\r\n");
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_post_ok(HttpRequest& req, HttpResponse& res)
{
	acl::string buf;
	if (!req.getBody(buf)) {
		printf("getBody error!\r\n");
		return false;
	}

	printf("body: %s\r\n", buf.c_str());
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_get_json(HttpRequest&, HttpResponse& res)
{
	acl::json json;
	acl::json_node& root = json.get_root();
	root.add_number("code", 200)
		.add_text("status", "+ok")
		.add_child("data",
			json.create_node()
				.add_text("name", "value")
				.add_bool("success", true)
				.add_number("number", 100));
	return res.write(json);
}

static bool http_get_xml(HttpRequest&, HttpResponse& res)
{
	acl::xml1 xml;
	acl::xml_node& root = xml.get_root();
	root.add_child("test6", true, "text6")
		.add_child("test61", true, "text61")
			.add_attr("name61_1", "value61_1")
			.add_attr("name61_2", "value61_2")
			.add_attr("name61_3", "value61_3")
			.add_child("test611", true, "text611")
				.add_attr("name611_1", "value611_1")
				.add_attr("name611_2", "value611_2")
				.add_attr("name611_3", "value611_3")
				.get_parent()
			.get_parent()
		.add_child("test62", true, "text62")
			.add_attr("name62_1", "value62_1")
			.add_attr("name62_2", "value62_2")
			.add_attr("name62_3", "value62_3")
			.get_parent()
		.add_attr("name6_1", "value6_1")
		.add_attr("name6_2", "value6_2")
		.add_attr("name6_3", "value6_3");

	return res.write(xml);
}

int main(int argc, char *argv[])
{
	acl::acl_cpp_init();

	http_service service;

	// setup the configure

	service.set_cfg_int(var_conf_int_tab)
		.set_cfg_int64(var_conf_int64_tab)
		.set_cfg_str(var_conf_str_tab)
		.set_cfg_bool(var_conf_bool_tab);

	// Register http handlers according different url path
	service.Get("/", http_get_default)
		.Get("/ok", http_get_ok)
		.Post("/ok", http_post_ok)
		.Get("/json", http_get_json)
		.Get("/xml", http_get_xml);

	if (argc >= 2 && strcasecmp(argv[1], "alone") == 0) {
		const char* addr = "|8887";

		acl::log::stdout_open(true);
		if (argc >= 4) {
			addr = argv[3];
		}
		printf("listen: %s\r\n", addr);

		service.run_alone(addr, argc >= 3 ? argv[2] : NULL);
	} else {
#if defined(_WIN32) || defined(_WIN64)
		const char* addr = ":8887";

		acl::log::stdout_open(true);
		printf("listen: %s\r\n", addr);

		service.run_alone(addr, NULL);
#else
		service.run_daemon(argc, argv);
#endif
	}

	return 0;
}
