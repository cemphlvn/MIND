# MIND'a Katkıda Bulunma

MIND'a katkıda bulunmaya gösterdiğiniz ilgi için teşekkürler.

> Diller / Languages: [English](CONTRIBUTING.md) · **Türkçe**

Bu belge, projenin temel ilkelerini ve kalite standartlarını koruyarak nasıl etkili katkıda
bulunulacağını açıklar.

## Davranış Kuralları

Bu proje [Contributor Covenant Davranış Kuralları](CODE_OF_CONDUCT.md)'nı izler. Katılarak bu
kurallara uymayı kabul etmiş olursunuz.

## Felsefe

Katkıda bulunmadan önce MIND'ın ne olduğunu ve ne olmadığını anlayın:

**MIND bir altyapıdır.** Şöyle tasarlanmıştır:
- Küçük
- Sıkıcı (boring)
- Deterministik
- Uzun ömürlü

**MIND şu değildir:**
- Özellik dolu bir çatı (framework)
- Bir model
- Bir ajan (agent)
- Bir demo

Katkılar MIND'ı daha *etkileyici* değil, daha *güvenilir* kılmalıdır.

## Katkı Türleri

### 1. Hata Raporları

İyi hata raporları şunları içerir:
- MIND sürümü
- Platform ve derleyici
- Asgari yeniden-üretim adımları
- Beklenen ve gerçek davranış
- İlgili yapılandırma (configuration)

Sorunları şuraya bildirin: [GitHub Issues](https://github.com/cemphlvn/MIND/issues)

### 2. Belgeler

Belge iyileştirmeleri her zaman memnuniyetle karşılanır:
- Açıklamalar
- Örnekler
- Yazım düzeltmeleri
- Çeviriler

### 3. Kod

Kod katkıları şunları yapmalıdır:
- Mevcut stile uymak
- Test içermek
- Belgeleri güncellemek
- Tüm CI denetimlerini geçmek
- Apache 2.0 başlıklarını içermek

### 4. Tasarım Önerileri

Önemli değişiklikler için önce bir tartışma açın:
- Sorunu tanımlayın
- Bir çözüm önerin
- Alternatifleri değerlendirin
- Geriye dönük uyumluluğu (backwards compatibility) ele alın

## Geliştirme Kurulumu

```bash
# Klonla
git clone https://github.com/cemphlvn/MIND.git
cd MIND

# Derle
mkdir build && cd build
cmake ..
make

# Test et
make test

# Biçimlendirmeyi denetle
make format-check
```

## Kod Stili

### C Stili

- C11 standardı
- 4 boşluk girinti
- Sekme (tab) yok
- 80 karakter satır sınırı (esnek)
- Süslü parantezler aynı satırda
- Fonksiyon ve değişkenlerde snake_case
- Sabitlerde UPPER_CASE

### Başlık Stili

Her dosya Apache 2.0 başlığını içermelidir:

```c
/*
 * Copyright 2026 The MIND Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

### Commit Mesajları

Biçim:
```
<tür>: <kısa özet>

<gerekirse ayrıntılı açıklama>

Signed-off-by: Your Name <your.email@example.com>
```

Türler:
- `fix:` Hata düzeltmeleri
- `feat:` Yeni özellikler
- `docs:` Belgeler
- `test:` Testler
- `refactor:` Kod yeniden yapılandırma
- `chore:` Bakım

## Pull Request Süreci

1. Depoyu **fork**'layın
2. `main`'den bir özellik dalı (feature branch) **oluşturun**
3. Değişikliklerinizi **uygulayın**
4. Kapsamlıca **test edin**
5. Bir pull request **gönderin**

### PR Gereksinimleri

- [ ] Tüm testler geçiyor
- [ ] Kod stil kurallarına uyuyor
- [ ] Belgeler güncellendi
- [ ] Commit mesajları biçime uyuyor
- [ ] Apache 2.0 başlıkları mevcut
- [ ] Yeni bağımlılık eklenmedi
- [ ] Geriye dönük uyumlu (ya da gerekçelendirilmiş kırıcı değişiklik)

### İnceleme Süreci

1. Otomatik CI denetimleri çalışır
2. En az bir bakımcı (maintainer) inceler
3. Geri bildirimleri ele alın
4. Bakımcı onaylar ve birleştirir (merge)

## Kabul Etmeyeceğimiz Şeyler

Proje bütünlüğünü korumak için şunları reddederiz:

- Determinizmi ihlal eden özellikler
- Dış bağımlılıklar
- GPU'ya özel kod yolları
- Rastgelelik (randomness) ya da entropi kaynakları
- Duvar-saati (wall-clock) kullanımı
- Gömmeler hakkında anlamsal varsayımlar
- Netliği feda eden "akıllıca" optimizasyonlar
- Belgelerde pazarlama dili

## Bakımcı (Maintainer) Olmak

MIND liyakate dayalı bir model izler:

1. Zaman içinde tutarlı biçimde katkıda bulunun
2. Proje ilkelerini anladığınızı gösterin
3. Başkalarının katkılarını incelemeye yardım edin
4. Mevcut bakımcılarca aday gösterilin
5. Bakımcı ekibinin uzlaşıyla onayı

Ayrıntılar için bkz. [GOVERNANCE.md](GOVERNANCE.md).

## Sorular?

- Bir [Discussion](https://github.com/cemphlvn/MIND/discussions) açın
- Mevcut [Issues](https://github.com/cemphlvn/MIND/issues) bölümünü inceleyin

## Tanınma

Tüm katkıda bulunanlar [CONTRIBUTORS.md](CONTRIBUTORS.md) içinde listelenir.

MIND'ı küçük, güvenilir ve uzun ömürlü tutmaya yardım ettiğiniz için teşekkürler.

---

> Bu, `CONTRIBUTING.md` dosyasının Türkçe çevirisidir. Bir tutarsızlık durumunda kaynak metin olarak
> İngilizce sürüm esas alınır.
</content>
