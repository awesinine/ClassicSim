#include "Whirlwind.h"

#include "CharacterStats.h"
#include "CombatRoll.h"
#include "CooldownReady.h"
#include "DeepWounds.h"
#include "Flurry.h"
#include "OverpowerBuff.h"
#include "Warrior.h"

Whirlwind::Whirlwind(Character* pchar) :
    Spell("Whirlwind", "Assets/warrior/Ability_whirlwind.png", pchar, RestrictedByGcd::Yes, 10.0, ResourceType::Rage, 25),
    warr(dynamic_cast<Warrior*>(pchar))
{}

bool Whirlwind::is_ready_spell_specific() const {
    return warr->in_berserker_stance();
}

void Whirlwind::spell_effect() {
    const int result = roll->get_melee_ability_result(warr->get_mh_wpn_skill(), pchar->get_stats()->get_mh_crit_chance());

    add_gcd_event();
    add_spell_cd_event();

    if (result == PhysicalAttackResult::MISS) {
        increment_miss();
        warr->lose_rage(static_cast<unsigned>(resource_cost));
        return;
    }
    if (result == PhysicalAttackResult::DODGE) {
        increment_dodge();
        warr->get_overpower_buff()->apply_buff();
        warr->lose_rage(static_cast<unsigned>(round(resource_cost * 0.25)));
        return;
    }
    if (result == PhysicalAttackResult::PARRY) {
        increment_parry();
        warr->lose_rage(static_cast<unsigned>(round(resource_cost * 0.25)));
        return;
    }

    double damage_dealt = damage_after_modifiers(warr->get_random_normalized_mh_dmg());

    if (result == PhysicalAttackResult::CRITICAL) {
        damage_dealt = round(damage_dealt * warr->get_ability_crit_dmg_mod());
        warr->melee_mh_yellow_critical_effect();
        add_crit_dmg(static_cast<int>(round(damage_dealt)), resource_cost, pchar->global_cooldown());
    }
    else if (result == PhysicalAttackResult::HIT) {
        warr->melee_mh_yellow_hit_effect();
        add_hit_dmg(static_cast<int>(round(damage_dealt)), resource_cost, pchar->global_cooldown());
    }

    warr->lose_rage(static_cast<unsigned>(resource_cost));
}
