These are some configuration bits that can be entered in [here](esphomeRemote/sharedRemoteConfig.yaml). This is C++-Code that is injected via the yml-File. Usually you can replace the entire section with the example given below.

# without TV
to configure the remote without a TV but with smart speakers:
```
  ## Home assistant tv and media player
  - lambda: |-
      SpeakerSetup soundBar = SpeakerSetup("media_player.beam", "Beam");
      std::vector<TVSetup> tvSetup = {};
      std::vector<SpeakerSetup> speakerSetup = {
        SpeakerSetup("media_player.kuechen_lautsprecher", "Küche"),
        SpeakerSetup("media_player.mass_kuechen_lautsprecher", "Küche_MASS"),
        SpeakerSetup("media_player.wohnzimmer_lautsprecher", "Wohnzimmer")
      };
      speakerGroup = new SonosSpeakerGroupComponent(displayUpdate);
      speakerGroup->setup(tvSetup, speakerSetup);
      return {speakerGroup};
    id: speaker_group_component
```

# with TV but without soundbar
?
