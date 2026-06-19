# MIND

**Yapay zekâ sistemleri için kendi kendine evrilen bir bilişsel çalışma-zamanı (runtime) ve bellek katmanı.**

> Diller / Languages: [English](README.md) · **Türkçe**

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   MIND, programlarınıza gömdüğünüz bir C kütüphanesidir.     │
│   Tek başına çalışan bir araç ya da çalıştırılabilir değildir.│
│                                                             │
│   Vektör gömmelerinden (embedding) deneyim biriktirir,      │
│   tekrar eden örüntüleri belleğe sıkıştırır,                │
│   ve zaman içinde kendi epistemik olgunluğunu açığa vurur.  │
│                                                             │
│   Modeller değiştirilebilir. Deneyim değildir.              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Bu Nedir

MIND, yapay zekâ uygulamaları için kalıcı bilişsel bellek sağlayan bir **kütüphanedir** (`libmind.a`).
Onu kendi programlarınıza bağlarsınız (link) — tek başına hiçbir şey yapmaz.

**Onu bellek için SQLite gibi düşünün:** küçük, gömülebilir, bağımlılıksız, yeniden başlatmalara dayanır.

### Çekirdek Yetenekler

| Yetenek | Açıklama |
|------------|----------|
| **Deneyim biriktirme** | Gömmeleri besleyin, sıkıştırılmış bellek oluşturun |
| **Yapısal merhamet** | Erken girdiler baskın olmaz; güven, tekrar gerektirir |
| **Zaman farkındalığı** | Duvar-saati değil, deneyimsel yaş izlenir |
| **Epistemik şeffaflık** | Plastisite, olgunluk, hız (velocity) açığa vurulur |
| **S2S kalibrasyonu** | Sunucudan-sunucuya birlikte-evrim için sinyal dışa aktarımı |
| **Kalıcılık** | Bilişsel durumun dosyaya kaydı/yüklenmesi |
| **Determinizm** | Aynı girdiler → her zaman aynı evrim |

### Bu Ne DEĞİLDİR

- ❌ Doğrudan çalıştırdığınız bir yürütülebilir değildir
- ❌ Bir sohbet botu veya ajan (agent) değildir
- ❌ Bir dil modeli değildir
- ❌ Bir gömme (embedding) üreticisi değildir

MIND gömmeleri *kabul eder* — onları *üretmez*. Gömmeleri elinizdeki kaynaktan getirirsiniz
(OpenAI, Ollama, yerel modeller vb.).

---

## Amaç

Şu niteliklere sahip **dayanıklı bir bellek katmanı** oluşturmak:

1. **Model değişimine dayanır** — LLM'i değiştirdiğinizde bellek kalıcı olur
2. **Yavaş evrilir** — Erken kirlenmeye direnir, tekrar gerektirir
3. **Kendini bilir** — Ne kadar güvenli/olgun olduğunu raporlar
4. **Kalibrasyona olanak tanır** — Birden çok örnek, inançları paylaşmadan kalibre olabilir

Tasarım değişmezi (invariant):

> **Sistem, sonradan katı olmaktansa erkenden yanılmayı tercih eder.**

---

## Nasıl Çalışır

### Mimari

```
┌─────────────────────────────────────────────────────────────┐
│                      UYGULAMANIZ                            │
│            (Python, Node, Rust, C — her ne ise)             │
├─────────────────────────────────────────────────────────────┤
│                          │                                  │
│                          ▼                                  │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │    Gömme     │───▶│    MIND      │───▶│   İpuçları   │  │
│  │  (float[])   │    │  (libmind.a) │    │   (güven)    │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
│                          │                                  │
│                          ▼                                  │
│                   ┌──────────────┐                          │
│                   │ .state dosyası│                         │
│                   │   (kalıcı)    │                         │
│                   └──────────────┘                          │
└─────────────────────────────────────────────────────────────┘
```

### Matematik

```
Plastisite ∈ (0.05, 1.0]    — Asla tamamen kristalleşmez (merhamet)
Sönüm:     p × 0.995        — Pekiştirmede (kristalleşme)
Toparlanma: p × 1.0005      — Yenilikte (açıklığı koru)
Eşik:      benzerlik > 0.85 — Pekiştirme ile yeni yuva (slot) arasında ayrım
Güven      = benzerlik × kararlılık × (ağırlık / (ağırlık + 1))
Olgunluk   = yaş × kararlılık
Hız        = Δplastisite / Δt
```

### Çekirdek Döngü

```c
// 1. Çalışma-zamanı ve durum oluştur
cr_runtime_t* rt = cr_runtime_create(&config);
cr_state_t* st = cr_state_create(rt);

// 2. Deneyim besle (gömmeler kendi kaynağınızdan)
cr_state_update(st, embedding, dim, delta_t);

// 3. İpucu için sorgula
cr_hint_t hint;
cr_state_query(st, query_embedding, dim, &hint);
// hint.confidence buna ne kadar güveneceğinizi söyler

// 4. Epistemik durumu gözlemle
cr_temporal_t t;
cr_state_temporal(st, &t);
// t.plasticity, t.age, t.maturity, t.velocity

// 5. Kalibrasyon sinyalini dışa aktar (S2S için)
cr_calibration_t cal;
cr_state_calibration(st, &cal);
// Akranlarla değiş-tokuş et — bellek paylaşılmaz, yalnızca meta-durum

// 6. Kalıcılaştır
cr_state_save(st, "memory.state");
cr_state_load(st, "memory.state");
```

---

## Hızlı Başlangıç

### Derleme

```bash
git clone https://github.com/cemphlvn/MIND
cd MIND
make
```

Bu şunları üretir:
- `build/libmind.a` — Bağlayacağınız (link) kütüphane
- `build/mind_example` — Tanıtım programı

### Örneği Çalıştır

```bash
make example
```

### Testleri Çalıştır

```bash
make test
```

### Kendi Projenizde Kullanım

**C/C++:**
```c
#include "cr.h"

// Şununla bağlayın: -lmind -lm
```

**Python (ctypes ile):**
```python
import ctypes
mind = ctypes.CDLL('./build/libmind.a')
# Tam sarmalayıcı (wrapper) için bkz. docs/INTEGRATION.md
```

**Diğer diller:**
C FFI'ı olan herhangi bir dil MIND'ı kullanabilir.

---

## Entegrasyon Örneği

Gerçek bir entegrasyon şöyle görünür:

```python
# example_integration.py (bunu siz yazarsınız)
import ollama
from mind_ffi import MindState  # FFI sarmalayıcısını siz yazarsınız

# MIND'ı başlat
mind = MindState(dim=768, slots=128)
mind.load("memory.state")  # Önceki deneyimi yükle

# Uygulama döngünüz
while True:
    user_input = input("> ")

    # 1. Modelinizden gömme alın
    emb = ollama.embeddings(model='nomic-embed-text', prompt=user_input)

    # 2. MIND'a besleyin
    mind.update(emb['embedding'], delta_t=1.0)

    # 3. Epistemik durumu kontrol edin
    temporal = mind.get_temporal()
    print(f"Olgunluk: {temporal.maturity:.2f}")

    # 4. Periyodik olarak kaydedin
    mind.save("memory.state")
```

---

## Proje Yapısı

```
MIND/
├── foundation/     # Katman 0: Saf matematik (asla değişmez)
├── core/           # Katman 1: Bilişsel bellek
├── external/       # Bağlamalar (bindings), entegrasyonlar, protokoller
├── docs/           # Belgeler
├── examples/
└── tests/
```

Mimari ayrıntılar için bkz. [LAYERS.md](LAYERS.md).

---

## Belgeler

| Belge | Açıklama |
|----------|----------|
| [LAYERS.md](LAYERS.md) | Katmanlı mimari (İngilizce) |
| [docs/architecture/API.md](docs/architecture/API.md) | Tam API başvurusu (İngilizce) |
| [docs/architecture/ARCHITECTURE.md](docs/architecture/ARCHITECTURE.md) | İç tasarım (İngilizce) |
| [docs/architecture/S2S_PROTOCOL.md](docs/architecture/S2S_PROTOCOL.md) | Sunucudan-sunucuya kalibrasyon (İngilizce) |
| [articles/mind_bir_onyukleme_teknigi.md](articles/mind_bir_onyukleme_teknigi.md) | MIND'ın bir önyükleme tekniği olarak okunması (Türkçe) |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Nasıl katkıda bulunulur (İngilizce) |
| [GOVERNANCE.md](GOVERNANCE.md) | Proje yönetişimi (İngilizce) |
| [SECURITY.md](SECURITY.md) | Güvenlik politikası (İngilizce) |

---

## Gelecek Yönelimler

### v0.2 — Sağlamlık
- Daha güvenli kalıcılık biçimi
- Durum sürümleme (versioning)
- Hata ayıklama doğrulamaları (assertions)

### v0.3 — Entegrasyon
- Python bağlamaları (`pip install mind`)
- Node.js bağlamaları
- Rust crate

### v0.4 — S2S Protokolü
- Prob (probe) kayıt defteri (paylaşılan koordinatlar)
- Proba-özgü yanıt dışa aktarımı
- Çok-akranlı kalibrasyon

### v1.0 — Kararlılık
- Uzun-vadeli API güvencesi
- Biçimsel doğrulama (formal verification)
- Üretim sertleştirmesi (hardening)

### Araştırma Yönelimleri
- Bellek dolduğunda tahliye (eviction) politikaları
- Gömme kayması (drift) tespiti
- Hiyerarşik bellek yapıları
- Federe (federated) kalibrasyon ağları

---

## Durum

**v0.1.0** — Çekirdek anlambilim donduruldu.

| Bileşen | Durum |
|-----------|--------|
| Çekirdek çalışma-zamanı | ✅ Tamam |
| Bellek/plastisite | ✅ Tamam |
| Zaman farkındalığı | ✅ Tamam |
| S2S kalibrasyon dışa aktarımı | ✅ Tamam |
| Kalıcılık | ✅ Tamam |
| S2S prob protokolü | ❌ Henüz değil |
| Dil bağlamaları | ❌ Henüz değil |
| Üretim sertleştirmesi | ❌ Henüz değil |

---

## Lisans

Apache Lisansı 2.0. Bkz. [LICENSE](LICENSE).

Ticari kullanıma izin verilir. Patent hakkı (grant) dahildir.

---

## Felsefe

MIND, yapay zekâ belleğine dair belirli bir duruşu somutlaştırır:

1. **Bellek, zekâdan önce gelir** — Performanstan önce yapı
2. **Erken kesinlik bir kusurdur** — İlk izlenimlere güvenme
3. **Zaman deneyimseldir** — Saatle değil, maruz kalmayla yaşlan
4. **Plastisite görünür olmalıdır** — Gizli otorite yok
5. **Modeller değiştirilebilir** — Deneyim, uygulamaları aşar

İlkelerin tamamı için bkz. [docs/site/principles.html](docs/site/principles.html).

---

> Bu, `README.md` dosyasının Türkçe çevirisidir. Bir tutarsızlık olması durumunda, kaynak metin
> olarak İngilizce sürüm esas alınır.
</content>
