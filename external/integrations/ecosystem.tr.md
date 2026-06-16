# Ekosistem Entegrasyon Notu — MIND Bir "Ajanik C Döngüsü"nün Çekirdeği Olarak

**Durum:** Tasarım/keşif notu. Üretim iddiası yoktur. Burada anlatılan tüm bileşenler MIND
çekirdeğinin **dışındadır**; çekirdek değişmeden kalır.

Bu not, aynı dizindeki [`README.md`](README.md)'deki asgari entegrasyon kalıbını
(`Kullanıcı Metni → Sağlayıcı → Gömme → MIND → İpuçları`) daha geniş bir sistemin içine yerleştirir.
Amaç, MIND'ı sürekli çalışan bir döngünün — bir *ajanik C döngüsünün* — bellek çekirdeği olarak
kullanmanın *mimarisini* tarif etmektir; çekirdeğe hiçbir şey eklemeden.

## Sınır: Çekirdek ile Çevre

[`LAYERS.md`](../../LAYERS.md) ve [`CONTRIBUTING.md`](../../CONTRIBUTING.md) açık bir sözleşme çizer.
MIND çekirdeğine **girmeyenler**: GPU'ya özel kod yolları, dış bağımlılıklar, rastgelelik,
duvar-saati. Aşağıdaki her şey — çıkarım, gömme üretimi, dış kaynaklardan topraklama (grounding),
ontoloji/graf kurma, hızlandırıcı seçimi — **çevre katmanda** (`external/` ve onun ötesinde) yaşar.
Bu bir eksiklik değil; çekirdeğin taşınabilirliğini ve determinizmini koruyan kasıtlı disiplindir.

## Döngü (mimari taslak)

```
   TOPRAKLAMA (grounding)        ÇIKARIM (inference)
   dış genel akışlar             yerel gömme üreticisi
   (ör. sandbox'lanmış           (ör. yerel bir model)
    tarayıcılar)                        │
        │  ham gözlem                   │ float[]
        ▼                               ▼
   ┌───────────────┐  gömme  ┌─────────────────────┐  ipucu  ┌──────────────────┐
   │  Çıkarıcı      │────────▶│   MIND (libmind.a)  │────────▶│  Üst katmanlar   │
   │  (metin→vektör)│         │  bellek + salınım   │  güven  │  ontoloji / graf │
   └───────────────┘         └─────────┬───────────┘         └──────────────────┘
                                       │ .state (kalıcı)
                                       ▼
                              tek bir deterministik çekirdek
```

- **Topraklama:** Dış, *genel* içerik bir çıkarıcıdan geçer; sonuç metin/gözlemdir. (Bir sandbox
  platformu — ör. headless tarayıcılar — bu katmanda kullanılabilir. Her kaynağın kendi kullanım
  şartlarına ve hız sınırlarına uyulur; bu, çekirdeğin değil çevrenin sorumluluğudur.)
- **Çıkarım/gömme:** Gözlemler bir gömme üreticisiyle vektöre çevrilir. MIND gömmeyi *üretmez*,
  yalnızca *kabul eder*; kaynak değiştirilebilir.
- **MIND:** Gömmeleri deneyime dönüştürür; plastisite/kristalleşme salınımıyla biriktirir; güven ve
  epistemik olgunluk raporlar; durumu `.state` olarak kalıcılaştırır. Çekirdek budur ve sabittir.
- **Üst katmanlar:** İpuçları ve güven sinyalleri, çekirdeğin **dışında** bir ontoloji/graf ya da
  başka bir uygulama tarafından tüketilir.

## Hızlandırıcı Seçimi Çevrenin Sorunudur

Ağır hesap (çıkarım, çevredeki bir salınım/özyineleme çekirdeği) bir hızlandırıcıya taşınabilir.
Burada **CUDA zorunlu değildir**. Köşegen, eleman-bazlı bir karmaşık özyineleme `associative scan`
(paralel önek) biçimine getirilebildiğinde, JAX AI Stack üzerinde **Pallas** ile yazılan bir TPU
çekirdeği eğilimle-uyumlu bir seçenektir; çıkarım için `vLLM-TPU` gibi araçlar kullanılabilir.

Dikkatli bir gözlem: bazı hızlandırıcı pod'ları fiziksel olarak **3 boyutlu torus** bir ara-bağlantıyla
örülüdür. Bir hesabın *matematiksel* torusu ile donanımın *ara-bağlantı* torusu **farklı şeylerdir**;
aralarında bir özdeşlik kurmak yanlış olur. Yine de her ikisi de topolojiye-duyarlı toplu işlemlerden
ve köşegen özyinelemenin doğal paralelliğinden yararlanır — yani bu, metafizik bir iddia değil, bir
**mühendislik fırsatıdır**. Hiçbiri MIND çekirdeğini etkilemez; çekirdek bu tercihlerden habersiz ve
bağımsız kalır.

> Daha ayrıntılı bir tartışma için bkz. [`articles/mind_bir_onyukleme_teknigi.md`](../../articles/mind_bir_onyukleme_teknigi.md), §5.

## Bu Notun Söylemediği

Bu not bir standart, bir API ya da bir uygulama önermez. Yalnızca MIND'ın daha büyük bir sistemde
*nasıl konumlandığını* — ve çekirdeğin neden sade kaldığını — sınırlandırır. Somut entegrasyonlar
(belirli sağlayıcılar, taşıma katmanları) bu dizinde ayrı, isteğe bağlı bileşenler olarak
yaşamalıdır.
</content>
