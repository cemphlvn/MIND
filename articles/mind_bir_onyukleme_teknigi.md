# Bir Önyükleme (Bootstrap) Tekniği Olarak MIND

## Model Değişim Çağında Dayanıklı Deneyim ve İş Pratiğine Yansımaları

**Durum:** Keşifsel teknik not. Üretim iddiası yoktur. Hiçbir standart, kıyaslama (benchmark) ya da
performans güvencesi öne sürülmez. Buradaki savlar mütevazıdır ve doğrulanmaları ampirik benzetim
gerektirir.

---

## Özet

Bu not, MIND'ı bir **önyükleme (bootstrap) tekniği** olarak inceler. "Önyükleme" terimini iki anlamda
kullanırız: (1) **teknik** anlamda — sistemin, dış denetim olmaksızın, yalnızca tekrar yoluyla deneyimi
kendi içinde kademeli olarak kristalleştirmesi; (2) **iş/girişim** anlamında — dış sermaye ya da satıcı
bağımlılığı olmadan, eldeki kaynaklarla kendi kendine biriken, taşınabilir bir altyapı. MIND'ın çekirdek
tasarımının (küçük, bağımlılıksız, deterministik C kütüphanesi) her iki okumayı da nasıl desteklediğini,
bunun çağdaş iş pratiklerinde ne anlama gelebileceğini ve neyin *araştırılabilir bir yayın* olarak ele
alınabileceğini ana hatlarıyla belirtiriz. Amaç abartı değil, sınırlandırmadır.

---

## 1. Motivasyon — Modeller Değişir, Deneyim Kalır

Modern yapay zekâ sistemlerinde değer, çoğunlukla değiştirilebilir bileşenlerde varsayılır: model
ağırlıkları, sağlayıcı API'leri, gömme (embedding) üreticileri. Oysa bunların hepsi geçicidir. Bir model
bir başkasıyla değiştirildiğinde, o modelle birlikte biriken çalışma deneyimi de tipik olarak kaybolur.

MIND'ın tasarım önermesi tek cümlede özetlenir: *modeller değiştirilebilir, deneyim değildir.* Sistem,
deneyimi modelden ayırır ve onu kalıcı, taşınabilir bir durum (`.state`) olarak saklar. Böylece deneyim,
belirli bir modele kilitlenmiş bir yan-ürün olmaktan çıkıp **kendi başına bir varlığa** dönüşür.

---

## 2. "Önyükleme" Terimi İki Anlamda

### 2.1 Teknik önyükleme

Klasik anlamıyla önyükleme, bir sistemin kendisini minimal bir çekirdekten başlatmasıdır. MIND'da bu,
plastisite döngüsüyle gerçekleşir: sistem yüksek plastisiteyle (açıklıkla) başlar ve yalnızca tekrar eden
örüntüler karşısında kademeli olarak kristalleşir. Hiçbir dış denetleyici "şuna inan" demez; yapı,
deneyimi *kendi içinde* damıtır.

### 2.2 İş anlamında önyükleme

Girişim diliyle önyükleme (bootstrapping), dış yatırım ya da bağımlılık olmadan, mevcut kaynakla büyümektir.
MIND bu anlamı mimari düzeyde yansıtır: sıfır bağımlılık, küçük yüzey alanı, gömülebilirlik. Bir kuruluş,
deneyim sermayesini üçüncü taraf bir platforma teslim etmeden, kendi sınırları içinde biriktirebilir.

Bu iki anlam tesadüfen örtüşmez: **bağımlılıksızlık ve yavaş, içsel birikim** her iki okumanın da ortak
çekirdeğidir.

---

## 3. Çekirdek Mekanizma — Salınım Döngüsü

MIND'ın öğrenme dinamiği küçük bir dizi değişmez sabit etrafında döner:

```
plastisite ∈ (ε, 1.0],  ε = 0.05   (merhamet tabanı — asla tamamen donmaz)
sönüm     = 0.995  (pekiştirmede kristalleşir)
toparlanma = 1.0005 (yenilikte açıklığı geri kazanır)
eşik      = 0.85   (pekiştirme ile yeni yuva arasındaki ayrım)
```

Bu, bir **salınım** tarif eder: yenilik plastisiteyi yukarı, tekrar aşağı iter; sistem ne tamamen donar
ne de tamamen akışkan kalır. Süreç deterministiktir (aynı girdiler → aynı evrim), sınırlıdır (bellek
`max_slots`'u aşmaz) ve zamanı duvar-saatiyle değil maruz kalmayla ölçer. Daha üst katmanlar bu çekirdeği
sürekli bir işlem hâline getirebilir; bu okumada MIND, bir **"ajanik C döngüsünün"** bellek çekirdeğidir —
dış dünyadan akan gözlemlerle topraklanan (grounding) ve kendi olgunluğunu raporlayan bir nüve.

---

## 4. İş Pratiklerinde Ne Anlama Gelir

### 4.1 Satıcı ve model bağımsızlığı
Apache-2.0, sıfır bağımlılık ve C-FFI taşınabilirliği, deneyim katmanını herhangi bir sağlayıcıdan
ayrıştırır. Model ya da bulut değiştirilebilir; deneyim yerinde kalır. Bu, satıcı kilidini (lock-in)
azaltan yapısal bir karardır — sözleşmeyle değil, mimariyle.

### 4.2 Deneyim bir varlık olarak
`.state` dosyası taşınabilir bir kurumsal varlıktır: yedeklenebilir, denetlenebilir, devredilebilir.
Bilanço diliyle, geçici bir hesaplama maliyeti, kalıcı bir **deneyim sermayesine** dönüşür.

### 4.3 Gizlilik-uyumlu kuruluşlar-arası kalibrasyon
S2S kalibrasyonu yalnızca meta-durum (plastisite, olgunluk, yaş) paylaşır; ham veri ya da bellek değil.
Bu, rakip ya da düzenlemeye tabi kuruluşların, özel veriyi açığa çıkarmadan tutarlılık denetimi
yapmasına olanak tanır. Paylaşılan şey *ne bildiğin* değil, *nasıl bildiğindir.*

### 4.4 Yönetişim ve denetim
"Erken kesinlik bir kusurdur" ilkesi, yapısal bir aşırı-güven (overconfidence) koruması sağlar. Sistem
kendi epistemik olgunluğunu (plastisite/olgunluk/hız) görünür kılar; bu, otomatik kararların ne kadar
olgun bir deneyime dayandığını denetlenebilir hâle getirir — yönetişim için somut bir sinyal.

### 4.5 İşletme maliyeti
Küçük, CPU üzerinde, bağımlılıksız bir kütüphane düşük işletme yükü demektir. Önyükleme ekonomisi tam da
budur: pahalı altyapı varsaymadan değer biriktirmek.

---

## 5. Ekosistem İçindeki Yer — Çekirdek ve Çevresi

MIND kasıtlı olarak dardır. Çıkarım (inference), gömme üretimi, dış kaynaklardan topraklama ve ontoloji
kurma gibi işler çekirdeğin **dışında** yaşar (bkz. `LAYERS.md`, `external/`). Katkı kuralları
(`CONTRIBUTING.md`) bu sınırı açıkça çizer: çekirdeğe **GPU'ya özel yollar, dış bağımlılıklar, rastgelelik
ve duvar-saati** girmez. Bu bir eksiklik değil, önyükleme disiplininin ta kendisidir — taşınabilirliği ve
determinizmi koruyan kasıtlı bir sadelik.

Ağır hesap, çevre katmanda — çekirdeğin dışında — hızlandırıcıya taşınabilir. Burada bir mühendislik notu
yerinde olur: çevredeki bir salınım çekirdeği (birim-modüllü / Fourier-HRR türü bir özyineleme) köşegen,
eleman-bazlı bir karmaşık özyinelemedir ve **associative scan** (paralel önek) biçimine getirilebildiğinde
modern hızlandırıcılarda verimli koşar. Bunun için **CUDA zorunlu değildir**: JAX AI Stack üzerinde
Pallas ile yazılan bir TPU çekirdeği eğilimle-uyumlu bir seçenektir; vLLM-TPU gibi araçlar çıkarımı
TPU'ya taşır.

İlgi çekici ama dikkatle ifade edilmesi gereken bir örtüşme vardır: Google TPU pod'ları, fiziksel olarak
**3 boyutlu torus** bir ara-bağlantı (ICI) ile örülüdür — örneğin Ironwood kuşağında her çip altı komşuya
bağlanır ve üç paralellik ekseni sunar. Bir hesabın *matematiksel* torusu ile donanımın *ara-bağlantı*
torusu **farklı şeylerdir**; aralarında metafizik bir özdeşlik kurmak yanlış olur. Yine de her ikisi de
topolojiye-duyarlı toplu işlemlerden (all-reduce / all-gather) ve köşegen özyinelemenin doğal
paralelliğinden yararlanır. Dolayısıyla bu örtüşme bir **mühendislik fırsatıdır**, bir teori değil — ve
MIND çekirdeği bu tercihlerden tümüyle habersiz ve bağımsız kalır. Donanım nötrlüğü, çekirdeğin değil
çevrenin sorunudur.

---

## 6. Neden "Araştırılabilir Bir Yayın"

MIND'ı bir araştırma nesnesi yapan şey, sınanabilir değişmezler öne sürmesidir:

- **Determinizm:** Aynı girdi dizisi her zaman aynı durum evrimini üretir — biçimsel olarak doğrulanabilir.
- **Sınırlılık:** Bellek `max_slots`'u aşamaz; kaynak kullanımı önceden kestirilebilir.
- **Merhamet değişmezi:** `plastisite ≥ ε` her zaman korunur.
- **Deneyimsel zaman:** Yaş yalnızca artar ve maruz kalmayla ölçülür.

Bu değişmezler, ölçülebilir hipotezlere kapı açar: erken-kontaminasyon direnci ölçülebilir mi? Kalibrasyon
sinyalleri akranlar arasında yakınsar mı? Topraklanmış deneyim, özyinelemeli kendi-kendine eğitimde görülen
**model çöküşünü** (model collapse) hafifletir mi? Bu sorular benzetimle çalışılabilir; not, yalnızca onları
çerçeveler.

İçerdeki S2S çalışması (bkz. `articles/cognitive_coevolution.md`) bu programın bir başka parçasıdır:
inanç paylaşmadan kalibrasyon. Birlikte, bu iki not, *zekâ büyütmeyi değil, zaman ve özerklik boyunca
epistemik kararlılığı* hedefleyen mütevazı bir araştırma gündemi oluşturur.

---

## 7. Sınırlar ve Başarısızlık Modları

Abartıdan kaçınmak için sınırların açıkça anılması gerekir:

- MIND **zekâ üretmez**; yalnızca epistemik kararlılık sağlar.
- Gömmeler hakkında **anlamsal varsayım yapmaz**; benzerlik salt geometriktir.
- Tek-iş parçacıklıdır (thread-safe değildir); eşzamanlılık dışarıdan ele alınmalıdır.
- Olası başarısızlıklar: yanlı problardan kaynaklı kalibrasyon kayması, yavaş yakınsama, kötü-niyetli
  kalibrasyon sinyalleri, zayıf sinyallerin aşırı-yorumlanması.

Bunlar reddetme değil, ihtiyat gerekçeleridir.

---

## 8. Sonuç

MIND, hem teknik hem iş anlamında bir önyükleme tekniği olarak okunabilir: dış bağımlılık olmadan,
eldeki deneyimden, yavaş ve deterministik biçimde değer biriktiren disiplinli bir çekirdek. Değeri zekâyı
büyütmekte değil, **modeller gelip geçerken deneyimi kalıcı kılmakta** yatar. Daha ileri her sav, spekülasyon
değil benzetim ve ampirik çalışma gerektirir.

---

## 9. Durum Notu

Bu belge hiçbir standart ya da uygulama önermez. Neyin makul, neyin erken olduğunu sınırlamak için
vardır. İçindeki tüm nicel ifadeler (sabitler, oranlar) MIND'ın mevcut `v0.1.0` çekirdeğinden alıntıdır
ve değişebilir.

---

## Kaynakça

1. Kanerva, P. (1988). *Sparse Distributed Memory.* MIT Press. — İçerik-adresli yuva belleğinin düşünsel
   atası.
2. Russell, S. & Wefald, E. (1991). *Do the Right Thing: Studies in Limited Rationality.* MIT Press. —
   Sınırlı rasyonalite; "erken kesinlik bir kusurdur" duruşunun kuramsal arka planı.
3. Kirkpatrick, J. ve diğ. (2017). "Overcoming catastrophic forgetting in neural networks." *PNAS*
   114(13), 3521–3526. — Plastisite/kararlılık ikilemi.
4. Shumailov, I. ve diğ. (2024). "AI models collapse when trained on recursively generated data."
   *Nature* 631, 755–759. — Özyinelemeli kendi-kendine eğitimde çöküş; topraklanmış deneyimin önemi.
5. Google Cloud (2025). "Inside the Ironwood TPU co-designed AI stack." — Bölüm 5'teki 3B-torus
   ara-bağlantı topolojisi için kaynak.

> Not: Bu makale Türkçe katkı olarak yazılmıştır. MIND belgelerinde dil çevirileri ve makaleler
> `CONTRIBUTING.md` uyarınca teşvik edilir.
</content>
