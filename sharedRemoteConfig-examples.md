<h1 align = "center">ESPHome Remote: Examples for sharedRemoteConfig </h1>

These are some configuration bits for common us cases that can be entered in [sharedRemoteConfig.yaml](esphomeRemote/sharedRemoteConfig.yaml). This is C++-Code that is injected via the yml-File, so if you are not used to it pay attention to all the weird charackters (,{},"| spaces - they all do matter). Usually you can replace the entire section with the example given below and just ajust the names of your entities.

# without TV
to configure the remote without a TV but with smart speakers:
```
  ## Home assistant tv and media player
  - lambda: |-
      SpeakerSetup soundBar = SpeakerSetup("media_player.beam", "Beam");
      std::vector<TVSetup> tvSetup = {};
      std::vector<SpeakerSetup> speakerSetup = {
        SpeakerSetup("media_player.kitchen", "Kitchen"),
        SpeakerSetup("media_player.sofa", "Living room")
      };
      speakerGroup = new SonosSpeakerGroupComponent(displayUpdate);
      speakerGroup->setup(tvSetup, speakerSetup);
      return {speakerGroup};
    id: speaker_group_component
```

# with TV but without soundbar
```  
  ## Home assistant tv and media player
  - lambda: |-
      std::vector<TVSetup> tvSetup = {
      TVSetup("media_player.your_smart_tv", "TV", NULL)
      };
      std::vector<SpeakerSetup> speakerSetup = {
        SpeakerSetup("media_player.kitchen", "Kitchen"),
        SpeakerSetup("media_player.sofa", "Living room")
      };
      speakerGroup = new SonosSpeakerGroupComponent(displayUpdate);
      speakerGroup->setup(tvSetup, speakerSetup);
      return {speakerGroup};
    id: speaker_group_component
```
