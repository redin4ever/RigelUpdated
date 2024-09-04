namespace OW {
    namespace offset {
        /*------------------------------------------------------*/
        static constexpr auto Address_viewmatrix_base = 0x387FDA0;  //Change  0x37C9DC0 // 0x387FDA0 // 0x3EC9DC0
        static constexpr auto Address_viewmatrix_base_test = 0x3F4C128;//Change  0x3E96018
        static constexpr auto Address_entity_base = 0x386B130; //Change 48 8b 05 ? ? ? ? 48 03 c9 89 54 c8 ? ff 0d ? ? ? ? 89 15 ? ? ? ? c7 87
        static constexpr auto offset_viewmatrix_ptr = 0x7E0;
        static constexpr auto offset_viewmatrix_xor_key = 0xB18AB348DA168725; //Change 0xE994D9A61837F61E
        /*------------------------------------------------------*/
        // sig : E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 4D ? 33 D2
        static constexpr auto HeapManager = 0x3938870; //Change  0x3882880
        static constexpr auto HeapManager_Var = 0x391C9B7; //Change  0x38672D3
        static constexpr auto HeapManager_Key = 0xDDCBE3A6634AEDD4; //Change  0x6DE0CEEB548519C3
        static constexpr auto HeapManager_Pointer = 0x160;
        static constexpr auto changefov = 0x39E41A8;  //0x392E1D8
        /*------------------------------------------------------*/
        static constexpr auto SensitivePtr = 0x2064;
        /*------------------------------------------------------*/
        static constexpr auto VisFN = 0x7D0B12; //Change 0x7AED32
        static constexpr auto VisRead = 0x391D900; //Change  0x3867900
        static constexpr auto Vis_Key = 0x20690E86123DFCF; //Change 0x6B6DE88A4C694441 / 0x20690E86123DFCF // 0x0DC41E1E4AE4C76A
        /*------------------------------------------------------*/
        static constexpr auto OutlineFN = 0x7BBF72; //Change   0x7B7EB2     
        static constexpr auto OutlineRead = 0x391D900; //Change  0x3867900
        static constexpr auto OutLine_Key = 0x3568AD7F9BBD3FEA; //Change  0x4DBBA6008B2945A9 / 
        /*------------------------------------------------------*/
        static constexpr auto g_player_controller = 0x11D0; //Change 
        /*------------------------------------------------------*/

    }
}