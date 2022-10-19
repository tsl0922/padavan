function profile_change(user_agent,req)
    if not user_agent or user_agent=='' then return end

    for name,profile in pairs(profiles) do
        local match=profile.match

        if profile.disabled~=true and  match and match(user_agent,req) then

            local options=profile.options

            local mtypes=profile.mime_types

            if options then for i,j in pairs(options) do cfg[i]=j end end

            if mtypes then
                if profile.replace_mime_types==true then
                    mime=mtypes
                else
                    for i,j in pairs(mtypes) do mime[i]=j end
                end
            end

            return name
        end
    end

    return nil
end

function profile_apply_config()
    load_plugins(cfg.profiles or "./profiles/",'profile')
end

function profile_http_handler(what,from,port,msg)
    plugins.profiles.current=profile_change(msg['user-agent'],msg)
end

function profile_sendurl(url,range) end

plugins['profiles']={}
plugins.profiles.disabled=false
plugins.profiles.name='Profiles'
plugins.profiles.desc='enables per-user-agent response customizations'
plugins.profiles.apply_config=profile_apply_config
plugins.profiles.http_handler=profile_http_handler
plugins.profiles.sendurl=profile_sendurl
plugins.profiles.current=nil

plugins.profiles.ui_config_vars=
{
    { "input",  "profiles" }
}

plugins.profiles.ui_actions=
{
    profiles_ui={ 'xupnpd - profiles ui action', function() end }       -- 'http://127.0.0.1:4044/ui/profiles_ui' for call
}

plugins.profiles.ui_vars={}                                             -- use whatever ${key} in UI HTML templates
