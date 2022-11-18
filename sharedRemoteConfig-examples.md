<h1 align = "center">ESPHome Remote: config Examples </h1>

These are some configuration bits for common use cases that can be put in [sharedRemoteConfig.yaml](esphomeRemote/sharedRemoteConfig.yaml). It is C++-Code that is injected via the yml-File, so if you are not used to it pay attention to all the weird charackters (,{},"| spaces - they all do matter). 

You should be able to replace the entire section with the example given below and simply adjust the names of your entities.

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
to configure the remote with a TV and with smart speakers but without a soundbar:

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
