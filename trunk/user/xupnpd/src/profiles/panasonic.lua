profiles['VIERA']=
{
    ['desc']='Panasonic Viera',

    -- "Panasonic MIL DLNA CP UPnP/1.0 DLNADOC/1.50"
    ['match']=function(user_agent)
                if string.find(user_agent,'Panasonic MIL',1,true)
                    then return true
                else
                    return false
                end
            end,

    ['options']=
    {
        ['dlna_headers']=true,
        ['dlna_extras']=true
    }
}
