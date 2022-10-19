function ui_api_v_2_call(args,data,ip,url,methtod)

    methtod = string.upper(methtod)
    route = string.split(url, '/')
    res = nil
    if methtod == "GET" then
      if route[1] == 'playlist' then
        res = {}
        local d=util.dir(cfg.playlists_path)
        if d then
            table.sort(d)
            for i,j in ipairs(d) do
                if string.find(j,'.+%.m3u$') then
                    local fname=util.urlencode(j)
                    table.insert (res,{ name = j, id = string.gsub(j, ".m3u", '') } )
                    --http.send(string.format('<tr><td><a href=\'/ui/show?fname=%s&%s\'>%s</a> [<a href=\'/ui/remove?fname=%s&%s\'>x</a>]</td></tr>\n',fname,'',j,fname,''))
                end
            end
        end
      end
      if route[1] =='status' then
          res = {}
          res['uuid'] = http_vars.uuid
          res['description'] = http_vars.description
          res['uptime'] = http_vars.uptime()
          res['fname'] = http_vars.fname
          res['port'] = http_vars.port
          res['name'] = http_vars.name
          res['version'] = http_vars.version
          res['manufacturer_url'] = http_vars.manufacturer_url
          res['manufacturer'] = http_vars.manufacturer
          res['interface'] = http_vars.interface
          res['url'] = http_vars.url
      end
    end
    if methtod == "DELETE" then
      if route[1] == "playlist" then
        res = {success = false}
        if route[2] then
            local real_name=util.urldecode( route[2] ) .. ".m3u"
            local path=cfg.playlists_path
            if args.feed=='1' then path=cfg.feeds_path end
            if  os.remove(path..real_name) then
                core.sendevent('reload')
                res.success = true
            else
                res = nil
            end
        end
      end
    end




    if res then
        http_send_headers(200,'json')
	      http.send(json.encode(res))
    else
        http_send_headers(404)
        	http.send(json.encode(url))
    end
end
