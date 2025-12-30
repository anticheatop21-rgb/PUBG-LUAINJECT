UObject *GetObjectByClass(UClass *Class, bool Default)
{
    auto objects = UObject::GetGlobalObjects();
    const std::string defaultStr = "Default";

    for (int i = 0; i < objects.Num(); i++)
    {
        auto object = objects.GetByIndex(i);
        if (!object || !object->IsA(Class))
        {
            continue;
        }

        auto objectName = object->GetFullName();
        bool hasDefault = objectName.find(defaultStr) != std::string::npos;

        if ((Default && hasDefault) || (!Default && !hasDefault))
        {
            return object;
        }
    }
    return nullptr;
}


void MainThread2()
{
    UGameLuaEnv *LuaEnv = (UGameLuaEnv *)GetObjectByClass(UGameLuaEnv::StaticClass(), false);
    while (!LuaEnv)
    {
        sleep(1);
        LuaEnv = (UGameLuaEnv *)GetObjectByClass(UGameLuaEnv::StaticClass(), false);
    }

    if (LuaEnv)
    {
        FString luaScript = R"(
    local path = "/sdcard/Android/obb/com.tencent.ig/lua_output.txt"
    local file = io.open(path, "w")
    if file then
        file:write("1st Leaked By :@Anticheat02 (Telegram) ")
        file:close()
    end
    )";

        FString TwitterLoginBypassScript = R"lua(
    pcall(function()
        local logic_imsdk_deeplink_login = require("client.logic.login.logic_imsdk_deeplink_login")
        
        if not logic_imsdk_deeplink_login then
            return
        end
        
        local BP_ENUM_PLAYFORM_TWITTER = BP_ENUM_PLAYFORM_TWITTER or 42
        local BP_ENUM_IMSDK_CHANNEL_TWITTER = BP_ENUM_IMSDK_CHANNEL_TWITTER or 35
        
        local originalLoginViaSystemWebview = logic_imsdk_deeplink_login.LoginViaSystemWebview
        if originalLoginViaSystemWebview then
            logic_imsdk_deeplink_login.LoginViaSystemWebview = function(self, loginType)
                if loginType == BP_ENUM_PLAYFORM_TWITTER then
                    return false
                end
                return originalLoginViaSystemWebview(self, loginType)
            end
        end
        
        local originalBindViaSystemWebview = logic_imsdk_deeplink_login.BindViaSystemWebview
        if originalBindViaSystemWebview then
            logic_imsdk_deeplink_login.BindViaSystemWebview = function(self, loginType)
                if loginType == BP_ENUM_IMSDK_CHANNEL_TWITTER then
                    return false
                end
                return originalBindViaSystemWebview(self, loginType)
            end
        end
        
        local logic_imsdk_interface = require("client.logic.login.logic_imsdk_interface")
        if logic_imsdk_interface then
            local originalIMSDKLogin = logic_imsdk_interface.Login
            if originalIMSDKLogin then
                logic_imsdk_interface.Login = function(self, loginType, extraJson, skipLocalCacheCheck)
                    local IMSDKHelper = import("IMSDKHelper")
                    if IMSDKHelper then
                        local IMSDKHelperInstance = IMSDKHelper.GetInstance()
                        if IMSDKHelperInstance then
                            local imsdkLoginChannelId = IMSDKHelperInstance:ConvertTConndChannel2IMSDKChannel(loginType)
                            if loginType == BP_ENUM_PLAYFORM_TWITTER or imsdkLoginChannelId == BP_ENUM_IMSDK_CHANNEL_TWITTER then
                                IMSDKHelperInstance:SetMSDKConfig({IMSDK_TWITTER_LOGIN_USING_WEB = "false"}, false)
                            end
                        end
                    end
                    return originalIMSDKLogin(self, loginType, extraJson, skipLocalCacheCheck)
                end
            end
        end
        
        local IMSDKHelper = import("IMSDKHelper")
        if IMSDKHelper then
            local IMSDKHelperInstance = IMSDKHelper.GetInstance()
            if IMSDKHelperInstance then
                local originalSetMSDKConfig = IMSDKHelperInstance.SetMSDKConfig
                if originalSetMSDKConfig then
                    IMSDKHelperInstance.SetMSDKConfig = function(self, config, bSave)
                        if config and config.IMSDK_TWITTER_LOGIN_USING_WEB then
                            config.IMSDK_TWITTER_LOGIN_USING_WEB = "false"
                        end
                        return originalSetMSDKConfig(self, config, bSave)
                    end
                end
                
                local originalOriginalLogin = IMSDKHelperInstance.OriginalLogin
                if originalOriginalLogin then
                    IMSDKHelperInstance.OriginalLogin = function(self, extraJson, permissions, skipLocalCacheCheck)
                        local currentChannel = self:GetChannel()
                        if currentChannel and (string.lower(currentChannel) == "twitter" or currentChannel == "TWITTER") then
                            self:SetMSDKConfig({IMSDK_TWITTER_LOGIN_USING_WEB = "false"}, false)
                        end
                        return originalOriginalLogin(self, extraJson, permissions, skipLocalCacheCheck)
                    end
                end
            end
        end
    end)
)lua";

        LuaEnv->LuaDoString(luaScript);
        LuaEnv->LuaDoString(TwitterLoginBypassScript);
    }
}