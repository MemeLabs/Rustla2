#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

namespace rustla2 {

class Config {
 public:
  static Config& Get() {
    static Config config;
    return config;
  }

  void Init(const std::string& config_path);

  const std::string& GetAPI() { return api_; }

  const std::string& GetAPIWS() { return api_ws_; }

  const std::string& GetDBDB() { return db_db_; }

  const std::string& GetDBPath() { return db_path_; }

  const std::string& GetDonateDoURL() { return donate_do_url_; }

  const std::string& GetDonateLinodeURL() { return donate_linode_url_; }

  const std::string& GetDonatePaypalURL() { return donate_paypal_url_; }

  const std::vector<std::string>& GetEmotes() { return emotes_; }

  uint32_t GetEmoteSimilarityMinLength() {
    return emote_similarity_min_length_;
  }

  uint32_t GetEmoteSimilarityPrefixCheckSize() {
    return emote_similarity_prefix_check_size_;
  }

  uint32_t GetEmoteSimilarityMinEditDistance() {
    return emote_similarity_min_edit_distance_;
  }

  const std::string& GetGithubURL() { return github_url_; }

  const std::string& GetJWTSecret() { return jwt_secret_; }

  const std::string& GetJWTName() { return jwt_name_; }

  const std::string& GetJWTDomain() { return jwt_domain_; }

  time_t GetJWTTTL() { return jwt_ttl_; }

  bool GetJWTSecure() { return jwt_secure_; }

  uint16_t GetPort() { return port_; }

  time_t GetLivecheckInterval() { return livecheck_interval_; }

  const std::string& GetTwitchClientID() { return twitch_client_id_; }

  const std::string& GetTwitchClientSecret() { return twitch_client_secret_; }

  const std::string& GetTwitchRedirectURL() { return twitch_redirect_url_; }

  const std::string& GetGooglePublicAPIKey() { return google_public_api_key_; }

  const std::string& GetIPAddressHeader() { return ip_address_header_; }

  time_t GetStreamBroadcastInterval() { return stream_broadcast_interval; }

  time_t GetRustlerBroadcastInterval() { return rustler_broadcast_interval; }

  const std::string& GetSSLCertPath() { return ssl_cert_path_; }

  const std::string& GetSSLKeyPath() { return ssl_key_path_; }

  const std::string& GetSSLKeyPassword() { return ssl_key_password_; }

  const std::string& GetPublicPath() { return public_path_; }

 private:
  const std::unordered_map<std::string, std::string> ReadConfigFile(
      const std::string& path);

  bool AssignString(std::string* prop, const std::string& key,
                    const std::unordered_map<std::string, std::string>& config,
                    const std::string fallback = "");

  template <typename T>
  bool AssignUint(T* prop, const std::string& key,
                  const std::unordered_map<std::string, std::string>& config,
                  const uint64_t fallback = -1) {
    std::string value;
    if (AssignString(&value, key, config)) {
      *prop = stoull(value);
      return true;
    }

    if (fallback != -1) {
      *prop = fallback;
      return true;
    }

    return false;
  }

  std::string api_;
  std::string api_ws_;
  std::string db_db_;
  std::string db_path_;
  std::string donate_do_url_;
  std::string donate_linode_url_;
  std::string donate_paypal_url_;
  std::vector<std::string> emotes_;
  uint32_t emote_similarity_min_length_;
  uint32_t emote_similarity_prefix_check_size_;
  uint32_t emote_similarity_min_edit_distance_;
  std::string github_url_;
  std::string jwt_secret_;
  std::string jwt_name_;
  std::string jwt_domain_;
  time_t jwt_ttl_;
  bool jwt_secure_;
  uint16_t port_;
  time_t livecheck_interval_;
  std::string twitch_client_id_;
  std::string twitch_client_secret_;
  std::string twitch_redirect_url_;
  std::string google_public_api_key_;
  std::string ip_address_header_;
  time_t stream_broadcast_interval;
  time_t rustler_broadcast_interval;
  std::string ssl_cert_path_;
  std::string ssl_key_path_;
  std::string ssl_key_password_;
  std::string public_path_;
};

}  // namespace rustla2
